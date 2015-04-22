// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayDataStructs.generated.h"

/**
 * A struct to define basic properties of a gameplay object
 */
USTRUCT(Blueprintable)
struct FGameplayObjectData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	FString DisplayMesh;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float HealthPoints;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float Weight;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float SalvageValue;
};
