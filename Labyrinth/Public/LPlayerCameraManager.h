// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "LPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	virtual void UpdateCamera(float DeltaTime) override;
	
public:

	ALPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	float NormalFOV;

	float TargetingFOV;
	
};
