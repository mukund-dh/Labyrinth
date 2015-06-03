// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LPlayerController.h"
#include "Public/GameplayBase/LGameSession.h"


namespace
{
	const FString CustomMatchKeyword("Custom");
}

ALGameSession::ALGameSession(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ALGameSession::OnCreateSessionComplete);
		OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ALGameSession::OnDestroySessionComplete);

		OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ALGameSession::OnFindSessionsComplete);
		OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ALGameSession::OnJoinSessionComplete);

		OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &ALGameSession::OnStartOnlineGameComplete);
	}
}

void ALGameSession::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		// tell non-local players to start online game
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ALPlayerController* PC = Cast<ALPlayerController>(*It);
			if (PC && !PC->IsLocalController())
			{
				// TO-DO: Add this to the Player Controller
				//PC->ClientStartOnlineGame();
			}
		}
	}
}

void ALGameSession::HandleMatchHasStarted()
{
	// start online game locally and wait for completion
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			UE_LOG(LogOnlineGame, Log, TEXT("Starting session %s on server"), *GameSessionName.ToString());
			OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
			Sessions->StartSession(GameSessionName);
		}
	}
}

void ALGameSession::HandleMatchHasEnded()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionsInterace();
		if (Sessions.IsValid())
		{
			// tell the clients to end
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator; It; ++It)
			{
				ALPlayerController* PC = Cast<ALPlayerController>(*It);
				if (PC && !PC->IsLocalPlayerController())
				{
					// TO-DO: Add this to Player Controller
					//PC->ClientEndOnlineGame();
				}
			}

			// server is handled here
			UE_LOG(LogOnlineGame, Log, TEXT("Ending session %s on server"), *GameSessionName.ToString());
			Sessions->EndSession(GameSessionName);
		}
	}
}

bool ALGameSession::IsBusy() const
{
	if (HostSettings.IsValid() || SearchSettings.IsValid())
	{
		return true;
	}

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			EOnlineSessionState::Type GameSessionState = Sessions->GetSessionState(GameSessionName);
			EOnlineSessionState::Type PartySessionState = Sessions->GetSessionState(PartySessionName);
			if (GameSessionState != EOnlineSessionState::NoSession || PartySessionState != EOnlineSessionState::NoSession)
			{
				return true;
			}
		}
	}
}

EOnlineAsyncTaskState::Type ALGameSession::GetSearchResultStatus(int32& SearchResultIdx, int32& NumSearchResults)
{
	SearchResultIdx = 0;
	NumSearchResults = 0;

	if (SearchSettings.IsValid())
	{
		if (SearchSettings->SearchState == EOnlineAsyncTaskState::Done)
		{
			SearchResultIdx = CurrentSessionParams.BestSessionIdx;
			NumSearchResults = SearchSettings->SearchResults.Num();
		}

		return SearchSettings->SearchState;
	}

	return EOnlineAsyncTaskState::NotStarted;
}

const TArray<FOnlineSessionSearchResult> & ALGameSession::GetSearchResults() const
{
	return SearchSettings->SearchResults;
}

void ALGameSession::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *SessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionsInterface();
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	OnCreatePresenceSessionComplete().Broadcast(SessionName, bWasSuccessful);
}

void ALGameSession::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnDestroySessionComplete %s bSuccess: %d"), *SessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		HostSettings = NULL;
	}
}

bool ALGameSession::HostSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		CurrentSessionParams.SessionName = SessionName;
		CurrentSessionParams.bIsLan = bIsLAN;
		CurrentSessionParams.bIsPresenceEnabled = bIsPresence;
		CurrentSessionParams.UserId = UserId;
		MaxPlayers = MaxNumPlayers;

		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
		{
			HostSettings = MakeShareable(new FLOnlineSessionSettings(bIsLAN, bIsPresence, MaxPlayers));
			HostSettings->Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_MATCHING_HOPPER, FString("TeamDeathmatch"), EOnlineDataAdvertisementType::DontAdvertise);
			HostSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_SESSION_TEMPLATE_NAME, FString("GameSession"), EOnlineDataAdvertisementType::DontAdvertise);
			HostSettings->Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);

			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			return Sessions->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
		}
	}

#if !UE_BUILD_SHIPPING
	else
	{
		OnCreatePresenceSessionComplete().Broadcast(GameSessionName, true);
	}
#endif
	return false;
}

void ALGameSession::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			UE_LOG(LogOnlineGame, Verbose, TEXT("Num Search Results: %d"), SearchSettings->SearchResults.Num());
			for (int32 SearchIdx = 0; SearchIdx < SearchSettings->SearchResults.Num(); SearchIdx++)
			{
				const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
				DumpSession(&SearchResult.Session);
			}

			OnFindSessionsComplete().Broadcast(bWasSuccessful);
		}
	}
}

void ALGameSession::ResetBestSessionVars()
{
	CurrentSessionParams.BestSessionIdx = -1;
}

void ALGameSession::ChooseBestSession()
{
	// Start searching where we left off
	for (int32 SessionIdx = CurrentSessionParams.BestSessionIdx; SessionIdx < SearchSettings->SearchResults.Num(); SessionIdx++)
	{
		CurrentSessionParams.BestSessionIdx = SessionIdx;
		return;
	}

	CurrentSessionParams.BestSessionIdx = -1;
}

void ALGameSession::StartMatchMaking()
{
	ResetBestSessionVars();
	ContinueMatchMaking();
}

void ALGameSession::ContinueMatchMaking()
{
	ChooseBestSession();
	if (CurrentSessionParams.BestSessionIdx >= 0 && CurrentSessionParams.BestSessionIdx < SearchSettings->SearchResults.Num())
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
			{
				OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
				Sessions->JoinSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, SearchSettings->SearchResults[CurrentSessionParams.BestSessionIdx]);
			}
		}
	}
	else
	{
		OnNoMatchesAvailable();
	}
}

void ALGameSession::OnNoMatchesAvailable()
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("Matchmaking complete, no sessions available."));
	SearchSettings = NULL;
}

void ALGameSession::FindSessions(TSharedPtr<FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		CurrentSessionParams.SessionName = SessionName;
		CurrentSessionParams.bIsLan = bIsLAN;
		CurrentSessionParams.bIsPresenceEnabled = bIsPresence;
		CurrentSessionParams.UserId = UserId;

		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && CurrentSessionParams.UserId.IsValid())
		{
			SearchSettings = MakeShareable(new FLOnlineSearchSettings(bIsLAN, bIsPresence));
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineComparisonOp::Equals);

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			Sessions->FindSessions(*CurrentSessionParams.UserId, SearchSettingsRef);
		}
	}
	else
	{
		OnFindSessionComplete(false);
	}
}

bool ALGameSession::JoinSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults)
{
	bool bResult = false;

	if (SessionIndexInSearchResults >= 0 && SessionIndexInSearchResults < SearchSettings->SearchResults.Num())
	{
		bResult = JoinSession(UserId, SessionName, SearchSettings->SearchResults[SessionIndexInSearchResults]);
	}

	return bResult;
}