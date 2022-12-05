// Fill out your copyright notice in the Description page of Project Settings.


#include "SDayCicleManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/DirectionalLightComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ASDayCicleManager::ASDayCicleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));
	SetRootComponent(DirectionalLight);
	if (DirectionalLight) {
		DirectionalLight->LightSourceAngle = 3.5f;
		DirectionalLight->Intensity = 3.75;
		DirectionalLight->bEnableLightShaftOcclusion = true;
		DirectionalLight->bCastCloudShadows = true;
		DirectionalLight->bCastShadowsOnClouds = true;
	}

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(DirectionalLight);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	if (RotatingMovement) {
		RotatingMovement->bAutoActivate = false;
	}
}

// Called when the game starts or when spawned
void ASDayCicleManager::BeginPlay()
{
	Super::BeginPlay();

	if (RotatingMovement) {
		float pitchRotation = bInvertRotation ? (360.f / (DayCicleInMinutes * 60.f)) : ((360.f / (DayCicleInMinutes * 60.f)) * -1.f);
		RotatingMovement->RotationRate = FRotator(pitchRotation, 0.f, 0.f);
		RotatingMovement->Activate();
	}

	if (HasAuthority()) {
		ReplicateActorRotation();

		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, this, &ASDayCicleManager::ReplicateActorRotation, SunRotationReplicationInterval, true, SunRotationReplicationInterval);
	}
}

// Called every frame
void ASDayCicleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Negative pitch means day and positive means night
	bool isDayTime = GetActorRotation().Pitch <= 0.f;
	if (bWasDayTime != isDayTime) {
		bWasDayTime = isDayTime;

		OnDayTimeSwitch.Broadcast(bWasDayTime);
	}
}

void ASDayCicleManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDayCicleManager, ActorRotation);
}

void ASDayCicleManager::OnRep_ActorRotation()
{
	SetActorRotation(ActorRotation);
}

void ASDayCicleManager::ReplicateActorRotation()
{
	ActorRotation = GetActorRotation();
}