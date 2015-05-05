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

	// Initialize the Data table to a default value
	static ConstructorHelpers::FObjectFinder<UDataTable> DataAssetFinder(TEXT("/Game/Blueprints/Book1"));
	GameObjectData = DataAssetFinder.Object;

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

	UpdateMesh();

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

void APickupAdvanced::UpdateMesh()
{
	FGameplayObjectData* ObjData = GameObjectData->FindRow<FGameplayObjectData>(DisplayName, "");
	if (!ObjData)
	{
		// The specified name was not found.
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, "The specified name was not found, genius");
		return;
	}

	LoadMesh = ObjData->DisplayMesh;
	UStaticMesh* DispMesh = LoadSelectedAsset();
	if (DispMesh)
		DisplayMeshComponent->SetStaticMesh(DispMesh);
}

void APickupAdvanced::UpdateDisplayMesh_Implementation()
{
	UpdateMesh();
}

bool APickupAdvanced::UpdateDisplayMesh_Validate()
{
	return true;
}

UStaticMesh* APickupAdvanced::LoadSelectedAsset()
{
	FStreamableManager& Streamable = ULabyrinthSingleton::Get().AssetLoader;
	const FStringAssetReference& AssetRef = LoadMesh.ToStringReference();

	if (LoadMesh.IsPending())
	{
		LoadMesh = Cast <UStaticMesh>(Streamable.SynchronousLoad(AssetRef));
	}

	return LoadMesh.Get();
}