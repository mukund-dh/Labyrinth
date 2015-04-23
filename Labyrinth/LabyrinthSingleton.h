// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Object.h"
#include "LabyrinthSingleton.generated.h"

/**
 * 
 */
UCLASS(Config=Game, notplaceable)
class LABYRINTH_API ULabyrinthSingleton : public UObject, public FTickerObjectBase
{
	GENERATED_BODY()
private:
	ULabyrinthSingleton(const FObjectInitializer& ObjectInitializer);

public:
	static ULabyrinthSingleton& Get();
	FStreamableManager AssetLoader;

	virtual bool Tick(float DeltaTime) override;
};
