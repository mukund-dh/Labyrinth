// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "PickupBase.h"



APickupBase::APickupBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weight = 0.0f;
	SalvageValue = 1.0f;

	DisplayMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	RootComponent = DisplayMeshComponent;
	DisplayMeshComponent->SetIsReplicated(true);
}

void APickupBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION_NOTIFY(APickupBase, DisplayMesh, COND_None, REPNOTIFY_Always);
}

void APickupBase::OnConstruction(const FTransform& Transform)
{
	DisplayMeshComponent->SetMobility(EComponentMobility::Movable);

	// Set the Display Mesh Component to the DisplayMesh
	DisplayMeshComponent->SetStaticMesh(DisplayMesh);

	// Enable the physics simulation
	DisplayMeshComponent->SetSimulatePhysics(true);
}

bool APickupBase::StartFocusItem_Implementation()
{
	DisplayMeshComponent->SetRenderCustomDepth(true);
	return true;
}

bool APickupBase::EndFocusItem_Implementation()
{
	DisplayMeshComponent->SetRenderCustomDepth(false);
	return false;
}

bool APickupBase::AddItemToInventory_Implementation(ALabyrinthCharacter* MainChar)
{
	return true;
}

void APickupBase::ServerAddItemToInventory_Implementation(ALabyrinthCharacter* MainChar)
{
	AddItemToInventory(MainChar);
}

bool APickupBase::ServerAddItemToInventory_Validate(ALabyrinthCharacter* MainChar)
{
	return true;
}

void APickupBase::UpdateDisplayMesh()
{
	DisplayMeshComponent->SetStaticMesh(DisplayMesh);
}