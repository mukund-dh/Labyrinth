// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LPlayerController.h"
#include "Public/LPlayerCameraManager.h"


ALPlayerController::ALPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ALPlayerCameraManager::StaticClass();
}

void ALPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}