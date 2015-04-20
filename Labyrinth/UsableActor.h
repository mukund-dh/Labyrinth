// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UsableActor.generated.h"

/**
 * A base class for any interactive object in the world.
 */
UCLASS()
class LABYRINTH_API AUsableActor : public AActor
{
	GENERATED_BODY()

public:

	AUsableActor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent)
	bool OnUsed(ACharacter* character);

	UFUNCTION(BlueprintNativeEvent)
	bool StartFocusItem();
	bool StartFocusItem_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	bool EndFocusItem();
	bool EndFocusItem_Implementation();
};
