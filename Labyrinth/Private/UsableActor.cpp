// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "UsableActor.h"


AUsableActor::AUsableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the replicate value to true
	// For Multiplayer
	SetReplicates(true);
}

bool AUsableActor::StartFocusItem_Implementation()
{
	// virtual function
	return true;
}

bool AUsableActor::EndFocusItem_Implementation()
{
	// virtual function
	return true;
}