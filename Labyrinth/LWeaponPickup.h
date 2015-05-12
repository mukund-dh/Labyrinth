// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "LWeaponPickup.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class LABYRINTH_API ALWeaponPickup : public AUsableActor
{
	GENERATED_BODY()

public:

	ALWeaponPickup(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ALWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComp;

	bool OnUsed_Implementation(ACharacter* character);
	
};
