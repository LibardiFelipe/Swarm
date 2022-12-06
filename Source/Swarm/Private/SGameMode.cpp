// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "SPlayerController.h"
#include "SCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASGameMode::ASGameMode()
{
	bStartPlayersAsSpectators = true;
}

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
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

	for (auto& player : InGamePlayerCharacters) {
		if (IsValid(player) && player->IsPlayerAlive()) {
			count++;
		}
	}

	return count;
}

uint8 ASGameMode::GetAliveHumansCount() const
{
	uint8 count = 0;

	for (auto& player : InGamePlayerCharacters) {
		if (IsValid(player) && player->IsPlayerHuman()) {
			count++;
		}
	}

	return count;
}

uint8 ASGameMode::GetAliveZombiesCount() const
{
	uint8 count = 0;

	for (auto& player : InGamePlayerCharacters) {
		if (IsValid(player) && !player->IsPlayerHuman()) {
			count++;
		}
	}

	return count;
}

void ASGameMode::AddCharacterToInGameArray(ASCharacter* character)
{
	InGamePlayerCharacters.AddUnique(character);
}

void ASGameMode::RemoveCharacterFromInGameArray(ASCharacter* character)
{
	InGamePlayerCharacters.Remove(character);
}

void ASGameMode::TryToStartGame()
{
	if (GetAliveTotalCount() >= 2 && CurrentGameState == EGameState::WAITING_FOR_PLAYERS) {
		CurrentGameState = EGameState::PREPARING_TO_START;

		CurrentGameMode = GetRandomGameMode();

		// TODO: Warn players that the game is about to start in X seconds

		if (CurrentGameMode == EGameMode::NONE) {
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, TEXT("O array de gamemodes disponiveis esta vazio!"));
			return;
		}

		// TODO: Call timer to infect players
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ASGameMode::InfectPlayers, StartGameCountdownSeconds, false, StartGameCountdownSeconds);
	}
}

void ASGameMode::CheckForWinner()
{
	if (CurrentGameState != EGameState::GAME_RUNNING)
		return;

	int32 humans = GetAliveHumansCount();
	int32 zombies = GetAliveZombiesCount();

	if (humans > 0 && zombies > 0)
		return;

	if (humans == 0 && zombies == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("O jogo empatou!"));
	}
	else if (zombies == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Os humanos venceram!"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Os zombies venceram!"));
	}

	CurrentGameState = EGameState::GAME_ENDED;
	RestartGame(RestarTimeAfterRoundEnded);
}

void ASGameMode::RestartGame(float timeToRestart)
{
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ASGameMode::RestartRound, timeToRestart, false, timeToRestart);
}

void ASGameMode::RespawnPlayer(ASPlayerController* pController)
{
	auto pawn = pController->GetPawn();
	if (IsValid(pawn))
		pawn->Destroy();

	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto spawnPoint = GetRandomSpawnPoint();
	auto character = GetWorld()->SpawnActor<ASCharacter>(GetClassById(pController->GetZombieClassId()), FTransform(spawnPoint->GetActorRotation(), spawnPoint->GetActorLocation(), FVector(1.f, 1.f, 1.f)), spawnInfo);
	pController->Possess(character);
}

EGameMode ASGameMode::GetRandomGameMode() const
{
	if (AllGameModes.Num() <= 0) return EGameMode::NONE;
	return AllGameModes[FMath::RandRange(0, AllGameModes.Num() - 1)];
}

void ASGameMode::InfectPlayers()
{
	// Infect players based on sorted gamemode
	switch (CurrentGameMode) {
		case EGameMode::INFECTION:
		{
			auto sorted = SortPlayers(1);
			TurnPlayers(sorted, EPlayerClass::ZOMBIE);
		} break;
		case EGameMode::MULTI_INFECTION:
		{
			auto sorted = SortPlayers(FMath::RandRange(1, GetAliveTotalCount() / 2));
			TurnPlayers(sorted, EPlayerClass::ZOMBIE);
		} break;
		case EGameMode::SWARM:
		{
			auto sorted = SortPlayers(GetAliveTotalCount() / 2);
			TurnPlayers(sorted, EPlayerClass::ZOMBIE);
		} break;
		case EGameMode::SURVIVOR:
		{
			auto sorted = SortPlayers(1);
			TurnPlayers(sorted, EPlayerClass::SURVIVOR);
			TurnPlayers(InGamePlayerCharacters, EPlayerClass::ZOMBIE, sorted);
		} break;
		case EGameMode::NEMESIS:
		{
			auto sorted = SortPlayers(1);
			TurnPlayers(sorted, EPlayerClass::NEMESIS);
		} break;
		default:
		{
			// APOCALYPSE MODE
			auto sorted = SortPlayers(GetAliveTotalCount() / 2);
			TurnPlayers(sorted, EPlayerClass::NEMESIS);
			TurnPlayers(InGamePlayerCharacters, EPlayerClass::SURVIVOR, sorted);
		}
	}

	CurrentGameState = EGameState::GAME_RUNNING;
}

void ASGameMode::RestartRound()
{
	for (auto& pc : InGamePlayerControllers) {
		if (pc->GetZombieClassId() > -1) {
			RespawnPlayer(pc);
		}
	}

	CurrentGameState = EGameState::WAITING_FOR_PLAYERS;
	TryToStartGame();
}

TSubclassOf<ASCharacter> ASGameMode::GetClassById(int32 classId) const
{
	return AvaiableZombieClasses[FMath::Clamp(classId, 0, AvaiableZombieClasses.Num() - 1)];
}

AActor* ASGameMode::GetRandomSpawnPoint() const
{
	return SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
}

TArray<ASCharacter*> ASGameMode::SortPlayers(int32 quanty)
{
	TArray<ASCharacter*> sortedPlayers;

	if (InGamePlayerCharacters.Num() < quanty)
		sortedPlayers;

	while (sortedPlayers.Num() < quanty) {
		auto sortedPlayer = InGamePlayerCharacters[FMath::RandRange(0, InGamePlayerCharacters.Num() - 1)];

		sortedPlayers.AddUnique(sortedPlayer);
	}

	return sortedPlayers;
}

void ASGameMode::TurnPlayers(TArray<ASCharacter*> players, EPlayerClass newClass, TArray<ASCharacter*> excludedPlayers)
{
	for (auto& player : players) {
		if (!excludedPlayers.Contains(player)) {
			player->TurnPlayer(newClass);
		}
	}
}
