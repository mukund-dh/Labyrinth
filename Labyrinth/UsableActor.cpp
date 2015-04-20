// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "UsableActor.h"


AUsableActor::AUsableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool AUsableActor::StartFocusItem_Implementation()
{
	return true;
}

bool AUsableActor::EndFocusItem_Implementation()
{
	return true;
}