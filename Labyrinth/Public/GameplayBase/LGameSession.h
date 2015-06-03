// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "Online.h"
#include "Public/GameplayBase/LabyrinthOnlineGameSettngs.h"
#include "LGameSession.generated.h"

struct FLGameSessionParams
{
	/** Name of session settings are stored with */
	FName SessionName;

	/** LAN Match */
	bool bIsLan;

	/** Presence enabled session */
	bool bIsPresenceEnabled;

	/** Id of player initiating the lobby */
	TSharedPtr<FUniqueNetId> UserId;

	/** Id of the player initiating lobby */
	int32 BestSessionIdx;

	FLGameSessionParams() : SessionName(NAME_None), bIsLan(false), bIsPresenceEnabled(false), BestSessionIdx(0)
	{

	}
};

/**
 * 
 */
UCLASS(config=Game)
class LABYRINTH_API ALGameSession : public AGameSession
{
	GENERATED_BODY()

	ALGameSession(const FObjectInitializer& ObjectInitializer);
	
protected:

	/** Delegate for creating a new session */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/** Delegate after starting a new session */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Transient properties of a session during game creation */
	FLGameSessionParams CurrentSessionParams;
	/** Current Host Settings */
	TSharedPtr <class FLOnlineSessionSettings> HostSettings;
	/** Current Search Settings */
	TSharedPtr <class FLOnlineSearchSettings> SearchSettings;

	/**
	 * Delegate fired when a session created request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful if true the async action completed without error, false if there was an error
	 */
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * Delegate fired when a session start request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful if true the async action completed without error, false if there was an error
	 */
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * Delegate fired when a session search query has completed
	 *
	 * @param bWasSuccessful if true the async action completed without error, false if there was an error
	 */
	void OnFindSessionComplete(bool bWasSuccessful);

	/**
	 * Delegate fired when a session joined request has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param Result 
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * Delegate fired when destroying an online session has completed
	 *
	 * @param SessionName the name of the session this callback is for
	 * @param bWasSuccessful if true the async action completed without error, false if there was an error
	 */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * Reset the variables which keep track of the session join attempts
	 */
	void ResetBestSessionVars();

	/**
	 * Choose the best session from a list of search results based on game criteria
	 */
	void ChooseBestSession();

	/**
	 * Entry point for matchmaking after search results are returned
	 */
	void StartMatchMaking();

	/**
	 * Return point after each attempt to join a search result
	 */
	void ContinueMatchMaking();

	/**
	 * Delegate triggered when no more search results are available
	 */
	void OnNoMatchesAvailable();

	/**
	 * Event triggered when a presence session is created
	 * 
	 * @param SessionName name of the created session
	 * @param bWasSuccessful was the event created successfully
	 */
	DECLARE_EVENT_TwoParams(ALGameSession, FOnCreatePresenceSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	/**
	* Event triggered when a session is joined
	*
	* @param Result the result of the join session
	*/
	DECLARE_EVENT_OneParam(ALGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/**
	* Event triggered when a session search completes
	*
	* @param Result the result of the join session
	*/
	DECLARE_EVENT_OneParam(ALGameSession, FOnFindSessionComplete, bool /*bWasSuccessful*/);
	FOnFindSessionComplete FindSessionCompleteEvent;

public:
	
	/** Default number of players allowed in the game */
	static const int32 DEFAULT_NUM_PLAYERS = 8;

	/**
	 * Host a new online session
	 *
	 * @param UserId user that initiated the request
	 * @param SessionName name of the session
	 * @param bIsLAN is this going to be hosted over lan
	 * @param bIsPresence is this session expected to create a presence session
	 * @param MaxNumPlayers Maximum number of players to allow in this session
	 *
	 * @return bool true if successful, else false
	 */
	bool HostSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	/**
	 * Find an online session
	 */
	void FindSessions(TSharedPtr<FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);

	/**
	 * Joins one of the sessions in the search results
	 */
	bool JoinSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults);

	/**
	 * Joins a session via the search results
	 */
	bool JoinSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** Returns true if any online async work is in progress */
	bool IsBusy() const;

	/**
	 * Get the search results found and the current search results being probed
	 */
	EOnlineAsyncTaskState::Type GetSearchResultStatus(int32& SearchResultIdx, int32& NumSearchResults);

	/**
	 * Get the search results
	 */
	const TArray<FOnlineSessionSearchResult> & GetSearchResults() const;

	/** Return the delegate fired when creating a presence session */
	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() { return CreatePresenceSessionCompleteEvent; }

	/** Return the delegate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }
	
	/** Return the delegate fired when the search of sessions completes */
	FOnFindSessionComplete& OnFindSessionsComplete() { return FindSessionCompleteEvent;  }

	/** Handle Starting the match */
	virtual void HandleMatchHasStarted() override;

	/** Handle when the match has ended */
	virtual void HandleMatchHasEnded() override;

	/**
	 * Travel to a session URL (as client) for a given session
	 */
	bool TravelToSession(int32 ControllerId, FName SessionName);

	/** Handles the various registered delegates */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
};
