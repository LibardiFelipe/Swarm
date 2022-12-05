// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	if (Camera) {
		Camera->bUsePawnControlRotation = true;
	}

	TPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TPMesh"));
	TPMesh->SetupAttachment(RootComponent);
	if (TPMesh) {
		TPMesh->bOwnerNoSee = true;
		TPMesh->bOnlyOwnerSee = false;
	}

	if (FPMesh) {
		FPMesh->bOwnerNoSee = false;
		FPMesh->bOnlyOwnerSee = true;
	}

	Health = HUMAN_MAX_HEALTH;
}

void ASCharacter::Destroyed()
{
	if (GameMode) {
		GameMode->RemoveCharacterFromInGameArray(this);
		GameMode->CheckForWinner();
	}

	Super::Destroyed();
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) {
		GameMode = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());

		if (GameMode) {
			GameMode->AddCharacterToInGameArray(this);

			FTimerHandle timerHandle;
			GetWorld()->GetTimerManager().SetTimer(timerHandle, GameMode, &ASGameMode::TryToStartGame, 1.f, false, 1.f);
		}
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::Turn);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::HandleJump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::HandleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::HandleUnCrouch);
}

void ASCharacter::CLIENT_ChangeFirstPersonSkeletalMesh_Implementation(EPlayerClass playerClass)
{
	FPMesh->SetSkeletalMesh(GetSkeletalMesh(playerClass));
	FPMesh->SetAnimInstanceClass(GetAnimInstanceClass(playerClass));
}

void ASCharacter::MoveForward(float value)
{
	if (!Controller || value == 0.f)
		return;

	const bool isOnGround = GetCharacterMovement()->IsMovingOnGround();
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation = FRotator(isOnGround ? 0.f : rotation.Pitch, rotation.Yaw, 0.f);

	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(direction, value);
}

void ASCharacter::MoveRight(float value)
{
	if (!Controller || value == 0.f)
		return;

	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation = FRotator(0.f, rotation.Yaw, 0.f);

	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(direction, value);
}

void ASCharacter::LookUp(float value)
{
	if (!Controller || value == 0.f)
		return;

	AddControllerPitchInput(value);
}

void ASCharacter::Turn(float value)
{
	if (!Controller || value == 0.f)
		return;

	AddControllerYawInput(value);
}

void ASCharacter::HandleCrouch()
{
	if (GetMovementComponent()->IsCrouching()) {
		if (bFlipFlopCrouching)
			UnCrouch();
	}
	else {
		Crouch();
	}
}

void ASCharacter::HandleUnCrouch()
{
	if (!bFlipFlopCrouching)
		UnCrouch();
}

void ASCharacter::HandleJump()
{
	if (GetMovementComponent()->IsCrouching()) {
		if (CanPlayerLeap()) {
			bCanLeap = false;
			SERVER_DoLeap();
		}
	}
	else {
		Jump();
	}
}

USkeletalMesh* ASCharacter::GetSkeletalMesh(EPlayerClass playerClass, bool bFirstPerson)
{
	// TODO: Change FP classes to TP when they are ready
	switch (playerClass) {
		case EPlayerClass::HUMAN: return bFirstPerson ? FP_HumanSkeletalMesh : TP_HumanSkeletalMesh;
		case EPlayerClass::ZOMBIE: return bFirstPerson ? FP_ZombieSkeletalMesh : TP_ZombieSkeletalMesh;
		case EPlayerClass::NEMESIS: return bFirstPerson ? FP_NemesisSkeletalMesh : TP_NemesisSkeletalMesh;
		default: return bFirstPerson ? FP_SurvivorSkeletalMesh : TP_SurvivorSkeletalMesh;
	}
}

TSubclassOf<UAnimInstance> ASCharacter::GetAnimInstanceClass(EPlayerClass playerClass)
{
	switch (playerClass) {
	case EPlayerClass::HUMAN: return HumanAnimInstance;
	case EPlayerClass::ZOMBIE: return ZombieAnimInstance;
	case EPlayerClass::NEMESIS: return NemesisAnimInstance;
	default: return SurvivorAnimInstance;
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, Health);
	DOREPLIFETIME(ASCharacter, PlayerClass);
	DOREPLIFETIME(ASCharacter, bRageMode);
	DOREPLIFETIME(ASCharacter, bHasInfectionShield);
	DOREPLIFETIME_CONDITION(ASCharacter, bCanLeap, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASCharacter, LeapCount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASCharacter, PlayerController, COND_OwnerOnly);
}

float ASCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// No need to beat the corpse
	if (!IsPlayerAlive())
		return 0.f;

	auto instigatorCharacter = Cast<ASCharacter>(EventInstigator->GetCharacter());
	if (!IsValid(instigatorCharacter) || instigatorCharacter->IsPlayerHuman() == IsPlayerHuman())
		return 0.f;

	if (GameMode->ShouldHumansBeInfected()) {
		if (bHasInfectionShield) {
			ChangeInfectionShield(false);
			return 0.f;
		}

		// TODO: Call infect player function

		return 99999.f;
	}
	
	auto trueDamage = bRageMode ? ((RageModeDamageReduction / 100.f) * DamageAmount) : DamageAmount;
	Health = FMath::Clamp(Health - trueDamage, -5.f, GetMaxHealth());

	// TODO: Apply knockback force

	if (!IsPlayerAlive()) {
		// TODO: Call kill player function
	}

	return trueDamage;
}

float ASCharacter::GetMaxHealth() const
{
	switch (PlayerClass) {
	case EPlayerClass::ZOMBIE: return ZOMBIE_MAX_HEALTH;
	case EPlayerClass::NEMESIS: return NEMESIS_MAX_HEALTH;
	case EPlayerClass::SURVIVOR: return SURVIVOR_MAX_HEALTH;
	default:
		return HUMAN_MAX_HEALTH;
	}
}

void ASCharacter::TurnPlayer(EPlayerClass newClass)
{
	CLIENT_ChangeFirstPersonSkeletalMesh(newClass);

	// TODO: Call a multicast funcion to play sounds
	// and particle effects.

	PlayerClass = newClass;
	Health = GetMaxHealth();

	if (GameMode) {
		GameMode->CheckForWinner();
	}
}

void ASCharacter::OnRep_PlayerClass()
{
	TPMesh->SetSkeletalMesh(GetSkeletalMesh(PlayerClass, false));
	TPMesh->SetAnimInstanceClass(GetAnimInstanceClass(PlayerClass));
}

float ASCharacter::GetKockbackResistance() const
{
	switch (PlayerClass) {
	case EPlayerClass::ZOMBIE: return ZombieKockbackResistance;
	case EPlayerClass::NEMESIS: return NemesisKockbackResistance;
	case EPlayerClass::SURVIVOR: return SurvivorKockbackResistance;
	default:
		return HumanKockbackResistance;
	}
}

void ASCharacter::ChangeInfectionShield(bool bActivate)
{
	// TODO: Call a MultiCast event with sound and particles

	bHasInfectionShield = bActivate;
}

bool ASCharacter::CanPlayerLeap() const
{
	return (bCanLeap && LeapCount > 0 && GetMovementComponent()->IsMovingOnGround())
		&& (PlayerClass == EPlayerClass::NEMESIS || PlayerClass == EPlayerClass::SURVIVOR || PlayerClass == EPlayerClass::ZOMBIE);
}

void ASCharacter::SERVER_DoLeap_Implementation()
{
	bCanLeap = false;
	LeapCount--;

	GetCharacterMovement()->Launch(GetControlRotation().Vector() * LeapStrength);

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ASCharacter::ResetLeapCooldown, LeapCooldown, false, LeapCooldown);
}

void ASCharacter::ResetLeapCooldown()
{
	bCanLeap = true;
}

bool ASCharacter::SERVER_DoLeap_Validate()
{
	return bCanLeap && LeapCount > 0;
}

void ASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<ASPlayerController>(NewController);
}

void ASCharacter::UnPossessed()
{
	Super::UnPossessed();

	PlayerController = nullptr;
}

