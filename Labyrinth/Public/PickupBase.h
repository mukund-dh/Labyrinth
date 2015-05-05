// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "LabyrinthCharacter.h"
#include "PickupBase.generated.h"

/**
 * A base class for all Pickups in the world. This a simple actor
 * which requires user inputs for the type of actor to fill the world
 * with.
 */
UCLASS()
class LABYRINTH_API APickupBase : public AUsableActor
{
	GENERATED_BODY()
	
public:
	
	/** Constructor */
	APickupBase(const FObjectInitializer& ObjectInitializer);

	/** The display mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = UpdateDisplayMesh, Category = "Pickup Properties")
	UStaticMesh* DisplayMesh;

	/** The Name to be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	FText DisplayName;

	/** The weight of the object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	float Weight;

	/** The salvage value of the object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	float SalvageValue;

	/** The display mesh component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Properties")
	UStaticMeshComponent* DisplayMeshComponent;

	/** The construction script */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Do the highlighting when in focus */
	bool StartFocusItem_Implementation();	

	/** Remove highlighting when out of focus */
	bool EndFocusItem_Implementation();

	/** Add this object to the character's inventory */
	UFUNCTION(BlueprintNativeEvent, Category = Inventory)
	bool AddItemToInventory(ALabyrinthCharacter* MainChar);
	virtual bool AddItemToInventory_Implementation(ALabyrinthCharacter* MainChar);

	/** A function to add this item into the inventory of the character who picks it up */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = Inventory)
	void ServerAddItemToInventory(ALabyrinthCharacter* MainChar);
	virtual void ServerAddItemToInventory_Implementation(ALabyrinthCharacter* MainChar);
	virtual bool ServerAddItemToInventory_Validate(ALabyrinthCharacter* MainChar);

protected:
	/** A function to update the Static Mesh Component */
	UFUNCTION()
	void UpdateDisplayMesh();
};
