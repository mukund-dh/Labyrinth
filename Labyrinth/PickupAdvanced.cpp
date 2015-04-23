// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthSingleton.h"
#include "GameplayDataStructs.h"
#include "PickupAdvanced.h"


APickupAdvanced::APickupAdvanced(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weight = 0.0f;
	SalvageValue = 1.0f;

	DisplayMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	RootComponent = DisplayMeshComponent;
	DisplayMeshComponent->SetIsReplicated(true);
}

void APickupAdvanced::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION_NOTIFY(APickupAdvanced, DisplayName, COND_None, REPNOTIFY_Always);
}

void APickupAdvanced::OnConstruction(const FTransform& Transform)
{
	DisplayMeshComponent->SetMobility(EComponentMobility::Movable);

	// Enable the physics simulation
	DisplayMeshComponent->SetSimulatePhysics(true);

	//DisplayName = GameObjectData->GetRowNames()[0];

	// Set the Display Mesh Component to the DisplayMesh
	//DisplayMeshComponent->SetStaticMesh(DisplayMesh);

}

bool APickupAdvanced::StartFocusItem_Implementation()
{
	DisplayMeshComponent->SetRenderCustomDepth(true);
	return true;
}

bool APickupAdvanced::EndFocusItem_Implementation()
{
	DisplayMeshComponent->SetRenderCustomDepth(false);
	return false;
}

bool APickupAdvanced::AddItemToInventory_Implementation(ALabyrinthCharacter* MainChar)
{
	return true;
}

void APickupAdvanced::ServerAddItemToInventory_Implementation(ALabyrinthCharacter* MainChar)
{
	AddItemToInventory(MainChar);
}

bool APickupAdvanced::ServerAddItemToInventory_Validate(ALabyrinthCharacter* MainChar)
{
	return true;
}

void APickupAdvanced::UpdateDisplayMesh()
{
	FGameplayObjectData* ObjData = GameObjectData->FindRow<FGameplayObjectData>(DisplayName, "");
	if (!ObjData)
	{
		// The specified name was not found.
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, "The specified name was not found, genius");
		return; 
	}

	UStaticMesh* DispMesh = LoadSelectedAsset(ObjData);
	if (DispMesh)
		DisplayMeshComponent->SetStaticMesh(DispMesh);
	
}

UStaticMesh* APickupAdvanced::LoadSelectedAsset(FGameplayObjectData* ObjData)
{
	FStreamableManager& Streamable = ULabyrinthSingleton::Get().AssetLoader;
	const FStringAssetReference& AssetRef = ObjData->DisplayMesh.ToStringReference();

	TAssetPtr<UStaticMesh> LoadedMesh = ObjData->DisplayMesh;

	if (ObjData->DisplayMesh.IsPending())
	{
		LoadedMesh = Cast <UStaticMesh>(Streamable.SynchronousLoad(AssetRef));
	}

	return LoadedMesh.Get();
}