// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDayCicleManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayTimeSwitch, bool, bIsDay);

class UDirectionalLightComponent;
class URotatingMovementComponent;
class UArrowComponent;

UCLASS()
class SWARM_API ASDayCicleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASDayCicleManager();
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
		FOnDayTimeSwitch OnDayTimeSwitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float DayCicleInMinutes{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		bool bInvertRotation{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float SunRotationReplicationInterval{ .1f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
		float DelayOffsetBeforeSendDayCicleSwitch{ 10.f };

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UDirectionalLightComponent* DirectionalLight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		URotatingMovementComponent* RotatingMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		UArrowComponent* ArrowComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_ActorRotation, Category = "Settings")
		FRotator ActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Settings")
		bool bWasDayTime{ false };

	void BroadcastDayCicleSwitch();
	
	UFUNCTION()
		void OnRep_ActorRotation();
	UFUNCTION()
		void ReplicateActorRotation();
};
