// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LWeapon.h"
#include "LImpactEffect.h"
#include "LWeaponInstant.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALWeaponInstant : public ALWeapon
{
	GENERATED_BODY()

	ALWeaponInstant(const FObjectInitializer& ObjectInitializer);
	
private:

	/*****************************************************************/
	/* Visual Handlers                                               */
	/*****************************************************************/
	
	void SimulateInstantHit(const FVector& Origin);

	void SpawnImpactEffects(const FHitResult& Impact);

	void SpawnTrailEffects(const FVector& EndPoint);

	/** Particle FX played when a surface is hit */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ALImpactEffect> ImpactTemplate;

	UPROPERTY(EditDefaultsOnly)
	FName TrailTargetParam;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* TrailFX;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* TracerFX;

	UPROPERTY(EditDefaultsOnly)
	float MinimumProjectileSpawnDistance;

	UPROPERTY(EditDefaultsOnly)
	int32 TracerRoundInterval;

	int32 BulletShotCount;

protected:

	/*****************************************************************/
	/* Damage Processing                                             */
	/*****************************************************************/

	virtual void FireWeapon() override;

	void DealDamage(const FHitResult& Impact, const FVector& ShooDir);

	bool ShouldDealDamage(AActor* TestActor) const;

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	void ProcessInstantHitConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);
	virtual void ServerNotifyHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);
	virtual bool ServerNotifyHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir);
	virtual void ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir);
	virtual bool ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitLocation)
	FVector HitOriginNotify;

	UFUNCTION()
	void OnRep_HitLocation();

	/*****************************************************************/
	/* Weapon Configuration                                          */
	/*****************************************************************/

	UPROPERTY(EditDefaultsOnly)
	float HitDamage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	float WeaponRange;

	/** Hit Verification: Allowed threshold for dot product between view direction and hit direction */
	UPROPERTY(EditDefaultsOnly)
	float AllowedViewDotHitDir;

	/** Hit Verification: Scale for bounding box of hit actor */
	UPROPERTY(EditDefaultsOnly)
	float ClientSideHitLeeway;

};
