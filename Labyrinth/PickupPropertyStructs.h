// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PickupPropertyStructs.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType, Category = "Pickup Properties")
struct FFoodPickup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	UStaticMesh* DisplayMesh;

	UPROPERTY(BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(BlueprintReadWrite)
	float Weight;

	UPROPERTY(BlueprintReadWrite)
	float SalvageValue;

	UPROPERTY(BlueprintReadWrite)
	float HealthPoints;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AActor> PickupClass;

	FFoodPickup()
	{
		DisplayMesh = NULL;
		//DisplayName = FText("");
		Weight = 0.0f;
		SalvageValue = 0.0f;
		HealthPoints = 0.0f;
	}
};