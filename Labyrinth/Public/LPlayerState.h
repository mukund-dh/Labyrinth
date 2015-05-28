// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "LPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ALPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void Reset() override;

	/** Set the team */
	virtual void ClientInitialize(class AController* InController) override;

	virtual void UnregisterPlayerWithSession() override;

	/** Setup new team and update pawn */
	void SetTeamNum(int32 NewTeamNumber);

	/** Player killed someone */
	void ScoreKill(ALPlayerState* Victim, int32 Points);

	/** Player Died */
	void ScoreDeath(ALPlayerState* KilledBy, int32 Points);

	/** Get Current Team */
	int32 GetTeamNum() const;

	/** Get number of kills */
	int32 GetKills() const;

	/** Get number of deaths */
	int32 GetDeaths() const;

	/** Get number of points */
	float GetScore() const;

	/** Get the number of bullets fired in this match */
	int32 GetNumBulletsFired() const;

	/** Get the number of rockets fired in this match */
	int32 GetNumRocketsFired() const;

	/** Get whether the player quit before this match */
	bool IsQuitter() const;

	/** Gets truncated player name to fit in death log and score boards */
	FString GetShortPlayerName() const;

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState);
	virtual void InformAboutKill_Implementation(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState);

	/** Broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState);
	virtual void BroadcastDeath_Implementation(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState);

	/** Replicate Team Colors. Update the player's mesh colors appropriately */
	UFUNCTION()
	void OnRep_TeamColor();

	// Stats about amount of ammo fired don't need to be server authenticated; just increment with local functions
	void AddBulletsFired(int32 NumBullets);
	void AddRocketsFired(int32 NumRockets);

	/** Set whether the player is a quitter */
	void SetQuitter(bool bInQuitter);

	virtual void CopyProperties(class APlayerState* PlayerState) override;

protected:

	/** Set the mesh colors based on the current teamnum variable */
	void UpdateTeamColors();

	/** Team Number */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamColor)
	int32 TeamNumber;

	/** Number of kills */
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/** Number of deaths */
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/** Number of bullets fired in this mathc */
	UPROPERTY()
	int32 NumBulletsFired;

	/** Number of rockets fired in this match */
	UPROPERTY()
	int32 NumRocketsFired;

	/** Whether user quit the match */
	UPROPERTY()
	uint8 bQuitter : 1;

	/** Helper for scoring points */
	void ScorePoints(int32 Points);
};
