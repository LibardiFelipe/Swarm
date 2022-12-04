// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enums.h"

#include "SGameMode.generated.h"

class ASPlayerController;
class ASCharacter;

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

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		EGameState GetCurrentGameState() const { return CurrentGameState; };

	void AddCharacterToInGameArray(ASCharacter* character);
	void RemoveCharacterFromInGameArray(ASCharacter* character);

	void TryToStartGame();

protected:
	EGameMode GetRandomGameMode() const;
	void InfectPlayers();

	TArray<ASCharacter*> SortPlayers(int32 quanty);

	void TurnPlayers(TArray<ASCharacter*> players, EPlayerClass newClass, TArray<ASCharacter*> excludedPlayers = {});

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		float StartGameCountdownSeconds{ 10.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		TArray<EGameMode> AllGameModes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		EGameMode CurrentGameMode = EGameMode::NONE;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		TArray<ASPlayerController*> InGamePlayerControllers;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		TArray<ASCharacter*> InGamePlayerCharacters;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		EGameState CurrentGameState = EGameState::WAITING_FOR_PLAYERS;
};
