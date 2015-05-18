// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "LFallDamage.generated.h"

/**
 * 
 */
UCLASS(const, Blueprintable, BlueprintType)
class LABYRINTH_API ULFallDamage : public UDamageType
{
	GENERATED_BODY()
	
public:

	ULFallDamage(const FObjectInitializer& ObjectInitializer);
	
	
};
