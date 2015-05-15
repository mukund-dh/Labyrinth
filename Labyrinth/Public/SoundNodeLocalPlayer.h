// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sound/SoundNode.h"
#include "SoundDefinitions.h"
#include "SoundNodeLocalPlayer.generated.h"

/**
 * Chooses different branch of sounds attached to locally controlled player
 * From the ShooterGame example
 */
UCLASS(hidecategories = Object, editinlinenew)
class LABYRINTH_API USoundNodeLocalPlayer : public USoundNode
{
	GENERATED_BODY()

public:

	virtual void ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances) override;

	virtual void CreateStartingConnectors(void) override;
	
	virtual int32 GetMaxChildNodes() const override;
	
	virtual int32 GetMinChildNodes() const override;

#if WITH_EDITOR

	virtual FString GetInputPinName(int32 PinIndex) const override;

#endif

};
