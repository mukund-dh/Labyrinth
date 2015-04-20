// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "LabyrinthCharacter.h"
#include "PickupPropertyStructs.h"
#include "PickupBase.generated.h"

/**
 * A base class for all Pickups in the world.
 */
UCLASS()
class LABYRINTH_API APickupBase : public AUsableActor
{
	GENERATED_BODY()
	
public:
	
	/** Constructor */
	APickupBase(const FObjectInitializer& ObjectInitializer);

	/** The display mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	UStaticMesh* DisplayMesh;

	/** The Name to be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Pickup Properties")
	FText DisplayName;

	/** The weight of the object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Pickup Properties")
	float Weight;

	/** The salvage value of the object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Pickup Properties")
	float SalvageValue;

	/** The construction script */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Do the highlighting when in focus */
	bool StartFocusItem_Implementation();	

	/** Remove highlighting when out of focus */
	bool EndFocusItem_Implementation();

	/** A function to add this item into the inventory of the character who picks it up */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = FooBar)
	bool AddItemToInventory(ALabyrinthCharacter* MainChar);
	virtual bool AddItemToInventory_Implementation(ALabyrinthCharacter* MainChar);

protected:
	/** The display mesh component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated)
	UStaticMeshComponent* DisplayMeshComponent;


};
