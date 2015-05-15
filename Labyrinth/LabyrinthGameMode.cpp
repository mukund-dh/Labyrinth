// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthGameMode.h"
#include "Public/LPlayerController.h"
#include "Public/LHUD.h"


ALabyrinthGameMode::ALabyrinthGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MainChar"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ALHUD::StaticClass();

	PlayerControllerClass = ALPlayerController::StaticClass();
}