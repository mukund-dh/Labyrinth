// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LImpactEffect.h"


// Sets default values
ALImpactEffect::ALImpactEffect()
{
	bAutoDestroyWhenFinished = true;

}

void ALImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Figure out what we hit
	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

UParticleSystem* ALImpactEffect::GetImpactFX(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SURFACE_DEFAULT:
		return DefaultFX;
	case SURFACE_FLESH:
		return FleshFX;
	default:
		return nullptr;
	}
}

USoundCue* ALImpactEffect::GetImpactSound(EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SURFACE_DEFAULT:
		return DefaultSound;
	case SURFACE_FLESH:
		return FleshSound;
	default:
		return nullptr;
	}
}