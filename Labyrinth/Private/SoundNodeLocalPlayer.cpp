// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/SoundNodeLocalPlayer.h"


void USoundNodeLocalPlayer::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
	// The accesses to the pawn will be unsafe when threading happens
	// deal with it first

	check(IsInGameThread());

	AActor* SoundOwner = ActiveSound.AudioComponent.IsValid() ? ActiveSound.AudioComponent->GetOwner() : NULL;
	APlayerController* PCOwner = Cast<APlayerController>(SoundOwner);
	APawn* PawnOwner = (PCOwner ? PCOwner->GetPawn() : Cast<APawn>(SoundOwner));

	const bool bLocallyControlled = PawnOwner && PawnOwner->IsLocallyControlled() && Cast<APlayerController>(PawnOwner->Controller);
	const int32 PlayIndex = bLocallyControlled ? 0 : 1;

	if (PlayIndex < ChildNodes.Num() && ChildNodes[PlayIndex])
	{
		ChildNodes[PlayIndex]->ParseNodes(AudioDevice, GetNodeWaveInstanceHash(NodeWaveInstanceHash, ChildNodes[PlayIndex], PlayIndex), ActiveSound, ParseParams, WaveInstances);
	}
}

void USoundNodeLocalPlayer::CreateStartingConnectors()
{
	InsertChildNode(ChildNodes.Num());
	InsertChildNode(ChildNodes.Num());
}

#if WITH_EDITOR
FString USoundNodeLocalPlayer::GetInputPinName(int32 PinIndex) const
{
	return (PinIndex == 0) ? TEXT("Local") : TEXT("REMOTE");
}
#endif

int32 USoundNodeLocalPlayer::GetMaxChildNodes() const
{
	return 2;
}

int32 USoundNodeLocalPlayer::GetMinChildNodes() const
{
	return 2;
}