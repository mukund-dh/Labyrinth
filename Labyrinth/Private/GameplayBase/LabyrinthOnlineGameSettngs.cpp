
#include "Labyrinth.h"
#include "Public/GameplayBase/LabyrinthOnlineGameSettngs.h"

FLOnlineSessionSettings::FLOnlineSessionSettings(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	NumPublicConnections = MaxNumPlayers;
	if (NumPublicConnections < 0)
	{
		NumPublicConnections = 0;
	}

	NumPrivateConnections = 0;
	bIsLANMatch = bIsLan;
	bShouldAdvertise = true;
	bAllowJoinInProgress = true;
	bAllowInvites = true;
	bUsesPresence = bIsPresence;
	bAllowJoinViaPresence = true;
	bAllowJoinViaPresenceFriendsOnly = true;
}

FLOnlineSearchSettings::FLOnlineSearchSettings(bool bSearchingLan, bool bSearchingPresence)
{
	bIsLanQuery = bSearchingLan;
	MaxSearchResults = 10;
	PingBucketSize = 50;

	if (bSearchingPresence)
	{
		QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	}
}

FLOnlineSearchSettingsEmptyDedicated::FLOnlineSearchSettingsEmptyDedicated(bool bSearchingLan, bool bSearchingPresence) : FLOnlineSearchSettings(bSearchingLan, bSearchingPresence)
{
	QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
	QuerySettings.Set(SEARCH_EMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);
}