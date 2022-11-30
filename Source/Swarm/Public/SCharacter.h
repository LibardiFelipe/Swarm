// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums.h"

#include "SCharacter.generated.h"

class ASPlayerController;
class ASGameMode;
class USkeletalMeshComponent;
class UCameraComponent;

UCLASS()
class SWARM_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
		float GetHealth() const { return Health; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
		bool IsPlayerAlive() const { return Health > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
		bool IsPlayerHuman() const { return PlayerClass == EPlayerClass::HUMAN || PlayerClass == EPlayerClass::SURVIVOR; };

	float GetKockbackResistance() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void LookUp(float value);
	void Turn(float value);

	void HandleCrouch();
	void HandleUnCrouch();
	void HandleJump();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Gameplay")
		bool bCanLeap{ true };
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Gameplay")
		int32 LeapCount{ 0 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		int32 MAX_LEAP_COUNT{ 3 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		float LeapCooldown{ 5.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		bool bFlipFlopCrouching{ false };

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* FPMesh = GetMesh();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* TPMesh;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Stats")
		ASPlayerController* PlayerController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "World")
		ASGameMode* GameMode;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Transient, Category = "Stats")
		float Health{ 0.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float HUMAN_MAX_HEALTH{ 250.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float ZOMBIE_MAX_HEALTH{ 3200.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float NEMESIS_MAX_HEALTH{ 12000.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float SURVIVOR_MAX_HEALTH{ 12000.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float HumanKockbackResistance{ 0.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float ZombieKockbackResistance{ 0.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float NemesisKockbackResistance{ 0.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float SurvivorKockbackResistance{ 0.f };

	// TODO: Turn into a replicatedUsing function with visual effects
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Stats")
		bool bRageMode{ false };

	// TODO: Turn into a replicatedUsing function with visual effects
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Stats")
		bool bHasInfectionShield{ false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
		float RageModeDamageReduction{ 50.f };

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Stats")
		EPlayerClass PlayerClass{ EPlayerClass::HUMAN };

	UFUNCTION(Server, Reliable, WithValidation, Category = "Gameplay")
		void SERVER_DoLeap();
	UFUNCTION()
		void SERVER_DoLeap_Implementation();
	UFUNCTION()
		bool SERVER_DoLeap_Validate();

	bool CanPlayerLeap() const;
	void ResetLeapCooldown();
	void ChangeInfectionShield(bool bActivate);
};
