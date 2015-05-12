// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameplayDataStructs.h"
#include "LWeaponPickup.h"
#include "LWeapon.generated.h"

UENUM()
enum class EWeaponState
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

UCLASS(ABSTRACT, Blueprintable)
class LABYRINTH_API ALWeapon : public AActor
{
	GENERATED_BODY()

public:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnEquip();

	virtual void OnUnEquip();

	virtual void OnEquipFinished();

	virtual void OnEnterInventory(ALabyrinthCharacter* NewOwner);

	virtual void OnLeaveInventory();

	bool IsEquipped() const;

	bool IsAttachedToPawn() const;

	/** Get weapon mesh (needs pawn owner to determine variant */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** Get Pawn Owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class ALabyrinthCharacter* GetPawnOwner() const;

	/** Set weapon's owning pawn */
	void SetOwningPawn(ALabyrinthCharacter* NewOwner);

	float GetEquipStartedTime() const;

	float GetEquipDuration() const;

	/** Last time this weapon was switched to */
	float EquipStartedTime;

	/** how much time the weapon needs to be equipped */
	float EquipDuration;

	bool bIsEquipped;

	bool bPendingEquip;

	FTimerHandle FiringTimerHandle;

	FTimerHandle EquipFinishedTimerHandle;

	/** The class to spawn in the level when dropped */
	UPROPERTY(EditDefaultsOnly, Category = "Game|Weapon")
	TSubclassOf<class ALWeaponPickup> WeaponPickupClass;

	/** The character socket to store this item at */
	EInventorySlot StorageSlot;

protected:

	/** Pawn Owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ALabyrinthCharacter* MyPawn;

	/** Weapon Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh;

	UFUNCTION()
	void OnRep_MyPawn();

	void AttachMeshToPawn(EInventorySlot Slot = EInventorySlot::Hands);

	void DetachMeshFromPawn();
	
public:	
	// Sets default values for this actor's properties
	ALWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE EInventorySlot GetStorageSlot() { return StorageSlot; }

	/***************************************************************************/
	/* Fire and Damage Handling                                                */
	/***************************************************************************/

	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

protected:

	bool CanFire() const;

	FVector GetAdjustedAim() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	// This allows us to forgo defining FireWeapon in the LWeapon.cpp
	virtual void FireWeapon() PURE_VIRTUAL(ALWeapon::FireWeapon, );

	UPROPERTY(EditDefaultsOnly)
	float TimeBetweenShots;

private:
	
	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	virtual void HandleFiring();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();
	virtual void ServerStartFire_Implementation();
	virtual bool ServerStartFire_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();
	virtual void ServerStopFire_Implementation();
	virtual bool ServerStopFire_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();
	virtual void ServerHandleFiring_Implementation();
	virtual bool ServerHandleFiring_Validate();

	void OnBurstStarted();

	void OnBurstFinished();

	bool bWantsToFire;

	EWeaponState CurrentState;

	bool bRefiring;

	float fLastFireTime;

	/***************************************************************************/
	/* Simulation and FX                                                       */
	/***************************************************************************/
	
private:
	
	UFUNCTION()
	void OnRep_BurstCounter();

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EquipAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireAnim;

	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleAttachPoint;

	bool bPlayingFireAnim;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

protected:

	virtual void SimulateWeaponFire();

	virtual void StopSimulatingWeaponFire();

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	UAudioComponent* PlayWeaponSound(USoundCue* SoundToPlay);

	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage* Animation);

};
