// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LWeapon.h"


ALWeaponPickup::ALWeaponPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = MeshComp;
}

bool ALWeaponPickup::OnUsed_Implementation(ACharacter* character)
{
	ALabyrinthCharacter* MyPawn = Cast<ALabyrinthCharacter>(character);
	if (MyPawn)
	{
		// Fetch the default variables of this class we are about to pick up and check if teh storage
		// slot is available
		if (MyPawn->WeaponSlotAvailable(WeaponClass->GetDefaultObject<ALWeapon>()->GetStorageSlot()))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;

			ALWeapon* NewWeapon = GetWorld()->SpawnActor<ALWeapon>(WeaponClass, SpawnInfo);

			MyPawn->AddWeapon(NewWeapon);

			Destroy();

			return true;
		}
	}

	return false;
}