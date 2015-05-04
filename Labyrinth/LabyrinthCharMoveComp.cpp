// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthCharMoveComp.h"


float ULabyrinthCharMoveComp::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ALabyrinthCharacter* CharOwner = cast<ALabyrinthCharacter>(PawnOwner);
	if (CharOwner)
	{
		// Slow down during targetting, but don't further reduce speed while crouching
		if (CharOwner->IsTargeting() && !CharOwner->GetMovementComponent()->IsCrouching())
		{
			MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
		}
		else if (CharOwner->IsSprinting())
		{
			MaxSpeed *= CharOwner->GetSprintingSpeedModifier();
		}
	}

	return MaxSpeed;
}

