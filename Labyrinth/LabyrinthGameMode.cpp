// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthGameMode.h"
#include "Public/LPlayerController.h"



ALabyrinthGameMode::ALabyrinthGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MainChar"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<AHUD> HUDClassFinder(TEXT("/Game/Blueprints/BP_HUD"));
	HUDClass = HUDClassFinder.Class;

	PlayerControllerClass = ALPlayerController::StaticClass();
}