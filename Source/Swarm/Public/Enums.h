// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerClass : uint8 {
    HUMAN       = 0 UMETA(DisplayName = "Human"),
    ZOMBIE      = 1 UMETA(DisplayName = "Zombie"),
    NEMESIS     = 2 UMETA(DisplayName = "Nemesis"),
    SURVIVOR    = 3 UMETA(DisplayName = "Survivor")
};

UENUM(BlueprintType)
enum class EGameMode : uint8 {
    NONE            = 0 UMETA(DisplayName = "None"),
    INFECTION       = 1 UMETA(DisplayName = "Infection"),
    MULTI_INFECTION = 2 UMETA(DisplayName = "Multi-Infection"),
    SWARM           = 3 UMETA(DisplayName = "Swarm"),
    NEMESIS         = 4 UMETA(DisplayName = "Nemesis"),
    SURVIVOR        = 5 UMETA(DisplayName = "Survivor"),
    APOCALYPSE      = 6 UMETA(DisplayName = "Apocalypse")
};

UENUM(BlueprintType)
enum class EGameState : uint8 {
    WAITING_FOR_PLAYERS = 0 UMETA(DisplayName = "Waiting for players"),
    PREPARING_TO_START  = 1 UMETA(DisplayName = "Preparing to start"),
    GAME_RUNNING        = 2 UMETA(DisplayName = "Game is running")
};