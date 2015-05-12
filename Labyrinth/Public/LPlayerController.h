// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "LPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALPlayerController : public APlayerController
{
	GENERATED_BODY()

	ALPlayerController(const FObjectInitializer& ObjectInitializer);

	void UnFreeze();
	
	
};
