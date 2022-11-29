// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "SPlayerController.h"
#include "SCharacter.h"

ASGameMode::ASGameMode()
{
	
}

void ASGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	InGamePlayerControllers.AddUnique(Cast<ASPlayerController>(NewPlayer));
}

void ASGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	InGamePlayerControllers.Remove(Cast<ASPlayerController>(Exiting));
}

bool ASGameMode::ShouldHumansBeInfected() const
{
	if (CurrentGameMode != EGameMode::INFECTION && CurrentGameMode != EGameMode::MULTI_INFECTION)
		return false;

	if (IsLastHumanAlive())
		return false;

	return true;
}

bool ASGameMode::CanAntidoteBeUsed() const
{
	if (CurrentGameMode == EGameMode::NEMESIS || CurrentGameMode == EGameMode::SURVIVOR || CurrentGameMode != EGameMode::APOCALYPSE)
		return false;

	if (IsLastHumanAlive())
		return false;

	return true;
}

uint8 ASGameMode::GetAliveTotalCount() const
{
	uint8 count = 0;

	for (auto& playerController : InGamePlayerControllers) {
		auto playerCharacter = Cast<ASCharacter>(playerController->GetCharacter());

		if (IsValid(playerCharacter) && playerCharacter->IsPlayerAlive()) {
			count++;
		}
	}

	return count;
}

uint8 ASGameMode::GetAliveHumansCount() const
{
	uint8 count = 0;

	for (auto& playerController : InGamePlayerControllers) {
		auto playerCharacter = Cast<ASCharacter>(playerController->GetCharacter());

		if (IsValid(playerCharacter) && playerCharacter->IsPlayerHuman()) {
			count++;
		}
	}

	return count;
}
