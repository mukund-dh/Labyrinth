// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthSingleton.h"

ULabyrinthSingleton::ULabyrinthSingleton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

ULabyrinthSingleton& ULabyrinthSingleton::Get()
{
	ULabyrinthSingleton *Singleton = Cast<ULabyrinthSingleton>(GEngine->GameSingleton);

	if (Singleton)
	{
		return *Singleton;
	}
	else 
	{
		return *ConstructObject<ULabyrinthSingleton>(ULabyrinthSingleton::StaticClass());
	}
}

bool ULabyrinthSingleton::Tick(float DeltaTime)
{
	return true;
}