// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PickupPropertyStructs.generated.h"

/**
 * The struct which contains the default properties of 
 * any food item in the world.
 */
USTRUCT(BlueprintType, Category = "Pickup Properties")
struct FFoodPickup
{
	GENERATED_USTRUCT_BODY()

	/** Store the Display Mesh */
	UPROPERTY(BlueprintReadWrite)
	UStaticMesh* DisplayMesh;

	/** Store the Display Name */
	UPROPERTY(BlueprintReadWrite)
	FText DisplayName;

	/** Store the Weight */
	UPROPERTY(BlueprintReadWrite)
	float Weight;

	/** Store the Salvage Value*/
	UPROPERTY(BlueprintReadWrite)
	float SalvageValue;

	/** Store the Health points the object gives out */
	UPROPERTY(BlueprintReadWrite)
	float HealthPoints;

	/** Store the base class of the object */
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AActor> PickupClass;

	FFoodPickup()
	{
		DisplayMesh = NULL;
		Weight = 0.0f;
		SalvageValue = 0.0f;
		HealthPoints = 0.0f;
	}
};