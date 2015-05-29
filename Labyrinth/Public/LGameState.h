// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "LGameState.generated.h"

typedef TMap<int32, TWeakObjectPtr<ALPlayerState> > RankedPlayerMap;

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALGameState : public AGameState
{
	GENERATED_BODY()

	ALGameState(const FObjectInitializer& ObjectInitializer);
	
public:
	
	/** Number of teams in the current game (Doesn't deprecate when no players are left in the team) */
	UPROPERTY(Transient, Replicated)
	int32 NumTeams;

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScores;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bIsTimerPaused;
	 
	/** gets ranked playerstate map for specific teams */
	void GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const;

	void RequestFinishAndExitToMainMenu();
};
