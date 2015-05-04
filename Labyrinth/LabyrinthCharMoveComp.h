// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "LabyrinthCharMoveComp.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ULabyrinthCharMoveComp : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual float GetMaxSpeed() const override;
	
};
