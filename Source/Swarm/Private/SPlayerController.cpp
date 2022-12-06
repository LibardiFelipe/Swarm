// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "SCharacter.h"

ASPlayerController::ASPlayerController()
{

}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, ZombieClassId, COND_OwnerOnly);
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
}

void ASPlayerController::SERVER_SetClassId_Implementation(int32 classId)
{
	if (ZombieClassId == classId)
		return;

	ZombieClassId = classId;

	if (GameMode) {
		if (GameMode->IsGameRunning()) {
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("Voce ira spawnar no proximo round!"));
			return;
		}

		GameMode->RespawnPlayer(this);
	}
}

void ASPlayerController::HandleMouseCursor(bool bShow)
{
	bShowMouseCursor = bShow;

	if (bShow) {
		SetInputMode(FInputModeGameAndUI());
	}
	else {
		SetInputMode(FInputModeGameOnly());
	}
}
