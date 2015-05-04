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
	TAssetPtr<UStaticMesh> DisplayMesh;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float HealthPoints;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float Weight;

	UPROPERTY(BlueprintReadOnly, Category = "G.O.D.")
	float SalvageValue;
};

UENUM()
enum class EInventorySlot : uint8
{
	/* For currently equipped items/weapons */
	Hands,

	/* For primary weapons on spine bone */
	Primary,

	/* Storage for small items like flashlight on pelvis */
	Secondary,
};

/**
 * A struct to define the properties of a "hit", or the damage dealt out to
 * the actor.
 */
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ActualDamage;

	UPROPERTY()
	UClass* DamageTypeClass;

	UPROPERTY()
	TWeakObjectPtr<class ALabyrinthCharacter> PawnInstigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	UPROPERTY()
	uint8 DamageEventClassID;

	UPROPERTY()
	bool bIsKilled;

private:

	UPROPERTY()
	uint8 EnsureReplicationByte;

	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:

	FTakeHitInfo() : ActualDamage(0), DamageTypeClass(nullptr), PawnInstigator(nullptr), DamageCauser(nullptr), DamageEventClassID(0), bIsKilled(false), EnsureReplicationByte(0)
	{

	}

	FDamageEvent& GetDamageEvent()
	{
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			if (PointDamageEvent.DamageTypeClass == nullptr)
			{
				PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return PointDamageEvent;

		case FRadialDamageEvent::ClassID:
			if (RadialDamageEvent.DamageTypeClass == nullptr)
			{
				RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return RadialDamageEvent;
			
		default:
			if (GeneralDamageEvent.DamageTypeClass == nullptr)
			{
				GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
			}
			return GeneralDamageEvent;
		}
	}

	void SetDamageEvent(const FDamageEvent& DamageEvent)
	{
		DamageEventClassID = DamageEvent.GetTypeID();
		switch (DamageEventClassID)
		{
		case FPointDamageEvent::ClassID:
			PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
			break;
		case FRadialDamageEvent::ClassID:
			RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
			break;
		default:
			GeneralDamageEvent = DamageEvent;
		}
	}

	void EnsureReplication()
	{
		EnsureReplicationByte++;
	}

};
