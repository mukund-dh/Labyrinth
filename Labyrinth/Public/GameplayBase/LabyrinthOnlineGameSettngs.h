
#pragma once

#include "OnlineSessionSettings.h"

/** 
 * General session settings for a game
 */
class FLOnlineSessionSettings : public FOnlineSessionSettings
{
public:

	FLOnlineSessionSettings(bool bIsLan = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FLOnlineSessionSettings() {}
};

/**
 * General search settings for a game
 */
class FLOnlineSearchSettings : public FOnlineSessionSearch
{
public:

	FLOnlineSearchSettings(bool bSearchingLan = false, bool bSearchingPresence = false);
	virtual ~FLOnlineSearchSettings() {}
};

/**
 * Search settings for an empty dedicated server to host a match
 */
class FLOnlineSearchSettingsEmptyDedicated : public FLOnlineSearchSettings
{
public:

	FLOnlineSearchSettingsEmptyDedicated(bool bSearchingLan = false, bool bSearchingPresence = false);
	virtual ~FLOnlineSearchSettingsEmptyDedicated() {}
};