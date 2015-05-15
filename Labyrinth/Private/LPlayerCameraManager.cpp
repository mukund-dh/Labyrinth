// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LPlayerCameraManager.h"


ALPlayerCameraManager::ALPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 65.0f;

	ViewPitchMin = -80.0f;
	ViewPitchMax = 80.0f;
	bAlwaysApplyModifiers = true;
}

void ALPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ALabyrinthCharacter* MyPawn = PCOwner ? Cast<ALabyrinthCharacter>(PCOwner->GetPawn()) : nullptr;
	if (MyPawn)
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
		SetFOV(DefaultFOV);
	}

	Super::UpdateCamera(DeltaTime);
}

