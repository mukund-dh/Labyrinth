// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LPlayerState.h"
#include "Public/LPlayerController.h"


ALPlayerState::ALPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void ALPlayerState::Reset()
{
	Super::Reset();

	// PlayerState persists across seamless travel. Keep the same teams
	// as previous match.
	NumKills = 0;
	NumDeaths = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void ALPlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

void ALPlayerState::ClientInitialize(class AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void ALPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdateTeamColors();
}

void ALPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void ALPlayerState::AddBulletsFired(int32 NumBullets)
{
	NumBulletsFired += NumBullets;
}

void ALPlayerState::AddRocketsFired(int32 NumRockets)
{
	NumRocketsFired += NumRockets;
}

void ALPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void ALPlayerState::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ALPlayerState* LPlayer = Cast<ALPlayerState>(PlayerState);
	if (LPlayer)
	{
		LPlayer->TeamNumber = TeamNumber;
	}
}

void ALPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		ALabyrinthCharacter* LCharacter = Cast<ALabyrinthCharacter>(OwnerController->GetCharacter());
		if (LCharacter != NULL)
		{
			// Trigger the color update...
			//LCharacter->UpdateTeamColorsAllMIDs();
		}
	}
}

int32 ALPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 ALPlayerState::GetKills() const
{
	return NumKills;
}

int32 ALPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float ALPlayerState::GetScore() const
{
	return Score;
}

int32 ALPlayerState::GetNumBulletsFired() const
{
	return NumBulletsFired;
}

int32 ALPlayerState::GetNumRocketsFired() const
{
	return NumRocketsFired;
}

bool ALPlayerState::IsQuitter() const
{
	return bQuitter;
}

void ALPlayerState::ScoreKill(ALPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ALPlayerState::ScoreDeath(ALPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void ALPlayerState::ScorePoints(int32 Points)
{
	// Change this to the LGameState class
	AGameState* const MyGameState = Cast<AGameState>(GetWorld()->GetGameState());
	if (MyGameState && TeamNumber >= 0)
	{
		// Handle the team scoring
		/*if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNUmber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;*/
	}

	Score += Points;
}

void ALPlayerState::InformAboutKill_Implementation(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState)
{
	// id can be null for bots
	if (KillerPlayerState->UniqueId.IsValid())
	{
		// Search for actual killer before calling OnKill()
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ALPlayerController* TestPC = Cast<ALPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player may not have an ID if it was create with CreateDebugPlayer
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				TSharedPtr<FUniqueNetId> LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->UniqueId)
				{
					//TestPC->OnKill();
				}
			}
		}
	}
}

void ALPlayerState::BroadcastDeath_Implementation(class ALPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ALPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		// all local players get death messages so they can update their huds
		ALPlayerController* TestPC = Cast<ALPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			//TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void ALPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALPlayerState, TeamNumber);
	DOREPLIFETIME(ALPlayerState, NumKills);
	DOREPLIFETIME(ALPlayerState, NumDeaths);
}

FString ALPlayerState::GetShortPlayerName() const
{
	if (PlayerName.Len() > 16)
	{
		return PlayerName.Left(16) + "...";
	}
	return PlayerName;
}
