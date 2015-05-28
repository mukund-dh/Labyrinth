// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthGameMode.h"
#include "Public/LPlayerController.h"
#include "Public/LPlayerState.h"
#include "Public/LHUD.h"
#include "Public/LGameState.h"


ALabyrinthGameMode::ALabyrinthGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MainChar"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ALHUD::StaticClass();

	PlayerControllerClass = ALPlayerController::StaticClass();
	PlayerStateClass = ALPlayerState::StaticClass();
	GameStateClass = ALGameState::StaticClass();

	MinRespawnDelay = 5.0f;

	bAllowBots = false;
	bNeedsBotCreation = false;
	bUseSeamlessTravel = true;
}

FString ALabyrinthGameMode::GetBotsCountOptionName()
{
	return FString(TEXT("Bots"));
}