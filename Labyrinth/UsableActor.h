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

	/**
	 * A function to implement what happens when a player uses
	 * this object. Should be implemented in the derived children's
	 * blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool OnUsed(ACharacter* character);
	bool OnUsed_Implementation(ACharacter* character);

	/**
	 * Function that implements what happens when the object becomes the focus
	 * of the current player. The default behaviour is to render a custom
	 * depth pass. Can be implemented in code, and overridden in the 
	 * derived children's blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool StartFocusItem();
	bool StartFocusItem_Implementation();

	/**
	 * Function that implements what happens when the object loses focus
	 * of the current player. The default behaviour is to render a custom
	 * depth pass. Can be implemented in code, and overridden in the 
	 * derived children's blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool EndFocusItem();
	bool EndFocusItem_Implementation();
};
