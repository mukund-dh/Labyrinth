// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LPlayerController.h"


ALPlayerController::ALPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ALPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}