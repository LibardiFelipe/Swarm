// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class ASGameMode;

UCLASS()
class SWARM_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SERVER_SetClassId(int32 classId);
	UFUNCTION()
		void SERVER_SetClassId_Implementation(int32 classId);

	int32 GetZombieClassId() const { return ZombieClassId; };

protected:
	UFUNCTION(BlueprintCallable)
		void HandleMouseCursor(bool bShow);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Gameplay")
		ASGameMode* GameMode;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Gameplay")
		int32 ZombieClassId{ -1 };
};
