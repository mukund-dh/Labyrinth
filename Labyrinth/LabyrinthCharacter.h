// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "UsableActor.h"
#include "PickupPropertyStructs.h"
#include "GameplayDataStructs.h"
#include "LWeapon.h"
#include "LabyrinthCharacter.generated.h"

UCLASS()
class LABYRINTH_API ALabyrinthCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn Skeletal Mesh */
	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;*/
	
	/** Boom to handle distance to player mesh. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;

	/** First Person Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent1P;

public:
	// Sets default values for this character's properties
	ALabyrinthCharacter(const FObjectInitializer& ObjectInitializer);

	/** An inventory of the food items our character is carrying */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Inventory)
	TArray<FFoodPickup> FoodInventory;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PawnClientRestart() override;

	/** Stop playing all anim montages */
	void StopAllAnimMontages();

	/***************************************************************************/
	/* Movement                                                                */
	/***************************************************************************/

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Client mapped to input */
	void OnCrouchToggle();

	/** Client mapped to input */
	void OnStartJump();

	/** Client mapped to input */
	void OnStopJump();

	/** Client mapped to input */
	void OnStartSprinting();

	/** Client mapped to input */
	void OnStopSprinting();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);
	virtual void ServerSetIsJumping_Implementation(bool NewJumping);
	virtual bool ServerSetIsJumping_Validate(bool NewJumping);

	void OnLanded(const FHitResult& Hit) override;

	/** Client/local call to update sprint state */
	void SetSprinting(bool NewSprinting);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetSprinting(bool NewSprinting);
	virtual void ServerSetSprinting_Implementation(bool NewSprinting);
	virtual bool ServerSetSprinting_Validate(bool NewSprinting);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	float GetSprintingSpeedModifier() const;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDamageType> DamageType;

	/***************************************************************************/
	/* Object Interaction                                                      */
	/***************************************************************************/

	/** Function that defines what happens when a player uses a usable object. */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = PlayerAbility)
	virtual void Use();
	void Use_Implementation();
	bool Use_Validate();

	/** Get which usable actor the player is focussing on */
	class AUsableActor* GetUsableInView();

	/** The maximum distance in which an object can be defined as usable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float MaxUseDistance;

	/** Does this character have a new focus? */
	bool bHasNewFocus;

	/** Pointer to the current usable actor in focus */
	AUsableActor* FocusedUsableActor;

	/***************************************************************************/
	/* Targeting                                                               */
	/***************************************************************************/

	void OnStartTargeting();

	void OnEndTargeting();

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool NewTargeting);
	virtual void ServerSetTargeting_Implementation(bool NewTargeting);
	virtual bool ServerSetTargeting_Validate(bool NewTargeting);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

	/***************************************************************************/
	/* Hitpoints                                                               */
	/***************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	void ConsumeFood(float AmountRestored);

	UFUNCTION(BlueprintCallable, Category = "Player Condition")
	bool IsAlive() const;

	void IncrementHunger();

	UPROPERTY(EditDefaultsOnly, Category = "Player Condition")
	float IncrementHungerAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Player Condition")
	float IncrementHungerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Player Condition")
	float CriticalHungerThreshold;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Player Condition")
	float Health;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Player Condition")
	float Hunger;

	UPROPERTY(EditDefaultsOnly, Category = "Player Condition")
	float MaxHunger;

	/***************************************************************************/
	/* Damage, Hit & Death                                                     */
	/***************************************************************************/

	/** Take Damage and handle death*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	void SetRagdollPhysics();

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	/** Holds hit data to replicate hits and death to clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	void ApplyFallDamage();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyFallDamage();
	virtual void ServerApplyFallDamage_Implementation();
	virtual bool ServerApplyFallDamage_Validate();

	bool bIsDying;

	/***************************************************************************/
	/* Weapons & Inventory                                                     */
	/***************************************************************************/

private:

	/** Attachpoint for active weapon or item in hands */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName WeaponAttachPoint;

	/** Attachpoint for item carried on belt/pelvis */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName PelvisAttachPoint;

	/** Attachpoint for item carried on the back */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName SpineAttachPoint;

	bool bWantsToFire;

	/** Distance to use when dropping inventory actors */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float DropItemDistance;

	void OnStartFire();

	void OnStopFire();

	void OnNextWeapon();

	void OnPrevWeapon();

	void OnEquipPrimaryWeapon();

	void OnEquipSecondaryWeapon();

	void StartWeaponFire();

	void StopWeaponFire();

	void DestroyInventory();

	void DropWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDropWeapon();
	virtual void ServerDropWeapon_Implementation();
	virtual bool ServerDropWeapon_Validate();

public:

	/** Check if the specified slot is available, limited to one item per type */
	bool WeaponSlotAvailable(EInventorySlot CheckSlot);

	/** Check if the pawn is allowed to fire weapons */
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsFiring() const;

	/** Return the socket names for attachements */
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;

	/** All items that the player has */
	UPROPERTY(Transient, Replicated)
	TArray<ALWeapon*> Inventory;

	void SpawnDefaultInventory();

	void SetCurrentWeapon(class ALWeapon* NewWeapon, class ALWeapon* LastWeapon = nullptr);

	void EquipWeapon(ALWeapon* Weapon);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon(ALWeapon* Weapon);
	virtual void ServerEquipWeapon_Implementation(ALWeapon* Weapon);
	virtual bool ServerEquipWeapon_Validate(ALWeapon* Weapon);

	UFUNCTION()
	void OnRep_CurrentWeapon(ALWeapon* LastWeapon);
	
	void AddWeapon(ALWeapon* Weapon);

	void RemoveWeapon(ALWeapon* Weapon);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ALWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<class ALWeapon>> DefaultInventoryClasses;

	/** Returns Mesh1P subobject **/
	/*FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }*/
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent1P; }

protected:

	/** Fire whatever you have to */
	void OnFire();

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	* Configures input for touchscreen devices if there is a valid touch interface for doing so
	*
	* @param	InputComponent	The input component pointer to bind controls to
	* @returns true if touch controls were enabled.
	*/
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);
};
