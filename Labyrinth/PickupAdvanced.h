// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableActor.h"
#include "LabyrinthCharacter.h"
#include "PickupAdvanced.generated.h"

/**
 * A Pickup which is going to derive its properties from a data
 * table. This is totally identical to the PickupBase in functionality
 * and in declaration. 
 */
UCLASS()
class LABYRINTH_API APickupAdvanced : public AUsableActor
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	APickupAdvanced(const FObjectInitializer& ObjectInitializer);

	/** The Name to be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = UpdateMesh, Category = "Pickup Properties")
	FName DisplayName;

	/** The Name to be displayed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Properties")
	UDataTable* GameObjectData;

	/** The weight of the object */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Properties")
	float Weight;

	/** The salvage value of the object */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Properties")
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

	/** A function to do the synchrounous swap of the asset */
	UStaticMesh* LoadSelectedAsset();

	/** A blueprint callable function which updates the display mesh */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = Inventory)
	void UpdateDisplayMesh();
	virtual void UpdateDisplayMesh_Implementation();
	virtual bool UpdateDisplayMesh_Validate();

	/** A function to update the Static Mesh Component */
	UFUNCTION()
	void UpdateMesh();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Defaults)
	TAssetPtr<UStaticMesh> LoadMesh;
};
