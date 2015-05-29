// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LGameState.h"
#include "Public/LPlayerController.h"
#include "Public/LPlayerState.h"


ALGameState::ALGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumTeams = 0;
	RemainingTime = 0;
	bIsTimerPaused = false;
}

void ALGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALGameState, NumTeams);
	DOREPLIFETIME(ALGameState, RemainingTime);
	DOREPLIFETIME(ALGameState, bIsTimerPaused);
	DOREPLIFETIME(ALGameState, TeamScores);
}

void ALGameState::GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const
{
	OutRankedMap.Empty();

	// first grab all the player states and rank them
	TMultiMap<int32, ALPlayerState*> SortedMap;
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		int32 Score = 0;
		ALPlayerState* CurPlayerState = Cast<ALPlayerState>(PlayerArray[i]);
		if (CurPlayerState && (CurPlayerState->GetTeamNum() == TeamIndex))
		{
			SortedMap.Add(FMath::TruncToInt(CurPlayerState->Score), CurPlayerState);
		}
	}

	// Sort the keys
	SortedMap.KeySort(TGreater<int32>());

	// Add them back to the ranked map
	OutRankedMap.Empty();
	
	int32 Rank = 0;
	for (TMultiMap<int32, ALPlayerState*>::TIterator It(SortedMap); It; ++It)
	{
		OutRankedMap.Add(Rank++, It.Value());
	}
}

void ALGameState::RequestFinishAndExitToMainMenu()
{
	if (AuthorityGameMode)
	{
		// we are the server, tell the game mode
		ALabyrinthGameMode* const GameMode = Cast<ALabyrinthGameMode>(AuthorityGameMode);
		if (GameMode)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		// we are the client, handle our own business
		// TO-DO: Add custom GameInstance class
		//UGameInstance* GI = GetGameInstance();
		//if (GI)
		//{
		//	//GI->RemoveSplitScreenPlayers();
		//}

		//ALPlayerController* const PrimaryPC = Cast<ALPlayerController>(GI->GetFirstLocalPlayerController());
		//if (PrimaryPC)
		//{
		//	check(PrimaryPC->GetNetMode() == ENetMode::NM_Client);
		//	// TO-DO: Add this to the Player Controller
		//	//PrimaryPC->HandleReturnToMainMenu();
		//}
	}
}