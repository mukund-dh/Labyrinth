// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LImpactEffect.generated.h"

UCLASS()
class LABYRINTH_API ALImpactEffect : public AActor
{
	GENERATED_BODY()
	
protected:
	UParticleSystem* GetImpactFX(EPhysicalSurface SurfaceType) const;
	USoundCue* GetImpactSound(EPhysicalSurface SurfaceType) const;

public:	
	// Sets default values for this actor's properties
	ALImpactEffect();

	virtual void PostInitializeComponents() override;

	/** FX spawned on standard materials */
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* FleshFX;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* DefaultSound;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* FleshSound;

	FHitResult SurfaceHit;
};
