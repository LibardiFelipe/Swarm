// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enums.h"

#include "SGameMode.generated.h"

class ASPlayerController;

UCLASS()
class SWARM_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		bool ShouldHumansBeInfected() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		bool CanAntidoteBeUsed() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		uint8 GetAliveTotalCount() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		uint8 GetAliveHumansCount() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		bool IsLastHumanAlive() const { return GetAliveHumansCount() == 1; };

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		EGameMode CurrentGameMode = EGameMode::NONE;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		TArray<ASPlayerController*> InGamePlayerControllers;
};
