// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LPlayerController.h"
#include "Public/LPlayerCameraManager.h"
#include "Public/LGameState.h"
#include "Online.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineEventsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#define ACH_FRAG_SOMEONE	TEXT("ACH_FRAG_SOMEONE")
#define ACH_SOME_KILLS		TEXT("ACH_SOME_KILLS")
#define ACH_LOTS_KILLS		TEXT("ACH_LOTS_KILLS")
#define ACH_FINISH_MATCH	TEXT("ACH_FINISH_MATCH")
#define ACH_LOTS_MATCHES	TEXT("ACH_LOTS_MATCHES")
#define ACH_FIRST_WIN		TEXT("ACH_FIRST_WIN")
#define ACH_LOTS_WIN		TEXT("ACH_LOTS_WIN")
#define ACH_MANY_WIN		TEXT("ACH_MANY_WIN")
#define ACH_SHOOT_BULLETS	TEXT("ACH_SHOOT_BULLETS")
#define ACH_SHOOT_ROCKETS	TEXT("ACH_SHOOT_ROCKETS")
#define ACH_GOOD_SCORE		TEXT("ACH_GOOD_SCORE")
#define ACH_GREAT_SCORE		TEXT("ACH_GREAT_SCORE")
#define ACH_PLAY_SANCTUARY	TEXT("ACH_PLAY_SANCTUARY")
#define ACH_PLAY_HIGHRISE	TEXT("ACH_PLAY_HIGHRISE")

static const int32 SomeKillsCount = 10;
static const int32 LotsKillsCount = 20;
static const int32 LotsMatchesCOunt = 5;
static const int32 LotsWinsCount = 3; 
static const int32 ManyWinsCount = 5;
static const int32 LotsBUlletsCount = 100;
static const int32 LotsRocketsCount = 10;
static const int32 GoodScoreCount = 10;
static const int32 GreatScoreCount = 15;


ALPlayerController::ALPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ALPlayerCameraManager::StaticClass();
	// TO-DO: Add the Cheat Manager
	// CheatClass = ULCheatManage::StaticClass();

	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;

	ServerSayString = TEXT("Say");
	LFriendUpdateTimer = 0.0f;
	bHasSentStartEvents = false;
}

void ALPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI Input
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ALPlayerController::OnToggleInGameMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ALPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ALPlayerController::OnHideScoreboard);
	InputComponent->BindAction("ConditionalCloseScoreboard", IE_Pressed, this, &ALPlayerController::OnConditionalCloseScoreboard);
	InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &ALPlayerController::OnToggleScoreboard);

	// Voice Chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &ALPlayerController::ToggleChatWindow);
}

void ALPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// FLStyle::Initialize();
	LFriendUpdateTimer = 0;
}

void ALPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ALPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (IsGameMenuVisible())
	{
		if (LFriendUpdateTimer > 0)
		{
			LFriendUpdateTimer -= DeltaTime;
		}
		else
		{
			// TO-DO: Add FLFriends class
			//TSharedPtr<class FLFriends> LFriends = LIngameMenu->GetLFriends();
			ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
			//if (LFriends.IsValid() && LocalPlayer && LocalPlayer->GetControllerId() >= 0)
			//{
			//		LFriends->UpdateFriends(LocalPlayer->GetControllerId());
			//}

			// The time between calls should be long enough that we don't trigger the
			// (0x80552C81) and we stay within the web api rate limit
			LFriendUpdateTimer = 4; 
		}
	}

	// If this is the first frame after the game has ended
	if (bGameEndedFrame)
	{
		bGameEndedFrame = false;

		// ONLY PUT CODE HERE WHICH YOU DON'T WANT TO BE DONE DUE TO HOST LOSS

		// Do we need to show the end of round scoreboard
		if (IsPrimaryPlayer())
		{
			//ALHUD* LHUD = GetLHUD();
			//if (LHUD)
			//{
			//		LHUD->ShowScoreboard(true, true);
			//}
		}
	}
}

void ALPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	// Build menu only after the game is internalized
	//LIngameMenu = MakeShareable(new FLIngameMenu());
	//LIngameMenu->Construct(Cast<ULocalPlayer>(InPlayer));
}

void ALPlayerController::QueryAchievements()
{
	// Precache Achievements
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
	{
		//IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		//if (OnlineSub)
		//{
		//IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
		const auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid())
		{
			TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
			if (UserId.IsValid())
			{
				//IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
				const auto Achievements = Online::GetAchievementsInterface();
				if (Achievements.IsValid())
				{
					Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &ALPlayerController::OnQueryAchievementsComplete));
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller"));
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("No valid identity interface"));
		}
		//}
		//else
		//{
		//	UE_LOG(LogOnline, Warning, TEXT("No default online subsytem"));
		//}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player; cannot read achievements"));
	}
}

void ALPlayerController::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	UE_LOG(LogOnline, Display, TEXT("ALPlayerController::OnQueryAchievementsComplete(bWasSuccessful = %s)"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE"));
}

void ALPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void ALPlayerController::FailedToSpawnPawn()
{
	if (StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void ALPlayerController::PawnPendingDestroy(APawn* P)
{
	LastDeathLocation = P->GetActorLocation();
	FVector CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);

	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void ALPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	UpdateSaveFileOnGameEnd(bIsWinner);
	UpdateAchievementsOnGameEnd();
	UpdateLeaderboardsOnGameEnd();

	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void ALPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

bool ALPlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(TEXT("DeathCamera"), true, GetPawn());

	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		const bool bIsBlocked = GetWorld()->LineTraceTest(PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bIsBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

bool ALPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void ALPlayerController::ServerCheat_Implementation(const FString& Msg)
{
	if (CheatManager)
	{
		ClientMessage(ConsoleCommand(Msg));
	}
}

void ALPlayerController::SimulateInputKey(FKey Key, bool bPressed)
{
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

void ALPlayerController::OnKill()
{
	UpdateAchievementProgress(ACH_FRAG_SOMEONE, 100.0f);

	const auto Events = Online::GetEventsInterface();
	const auto Identity = Online::GetIdentityInterface();

	if (Events.IsValid() && Identity.IsValid())
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (LocalPlayer)
		{
			int32 UserIndex = LocalPlayer->GetControllerId();
			TSharedPtr<FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
			if (UniqueID.IsValid())
			{
				ALabyrinthCharacter* Pawn = Cast<ALabyrinthCharacter>(GetCharacter());
				// If a player is dead, use location stored during pawn cleanup
				FVector Location = Pawn ? Pawn->GetActorLocation() : LastDeathLocation;
				ALWeapon* Weapon = Pawn ? Pawn->CurrentWeapon : 0;
				int32 WeaponType = 0; // TO-DO: Add AmmoType to LWeapon Weapon ? (int32)Weapon->GetAmmoType() : 0;

				FOnlineEventParms Params;

				Params.Add(TEXT("SectionId"), FVariantData((int32)0)); //unused
				Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // TO-DO: Determine game mode
				Params.Add(TEXT("DifficutyLevelId"), FVariantData((int32)0)); //unused

				Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0)); //unused
				Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)WeaponType));
				Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0)); //unused
				Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0)); //untracked
				Params.Add(TEXT("KillerTypeId"), FVariantData((int32)0)); // unused
				Params.Add(TEXT("LocationX"), FVariantData((Location.X)));
				Params.Add(TEXT("LocationY"), FVariantData((Location.Y)));
				Params.Add(TEXT("LocationZ"), FVariantData((Location.Z)));

				Events->TriggerEvent(*UniqueID, TEXT("KillOpponent"), Params);
			}
		}
	}
}

void ALPlayerController::UpdateAchievementProgress(const FString& Id, float Percent)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		//IOnlineSubsystem* OnlineSub = IOnlineSubsytem::Get();
		//if (OnlineSub)
		//{
		//IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
		const auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid())
		{
			TSharedPtr<FUniqueNetId> UserId = LocalPlayer->GetCachedUniqueNetId();

			if (UserId.IsValid())
			{
				const auto Achievements = Online::GetAchievementsInterface();
				if (Achievements.IsValid() && (!WriteObject.IsValid() || WriteObject->WriteState != EOnlineAsyncTaskState::InProgress))
				{
					WriteObject = MakeShareable(new FOnlineAchievementsWrite());
					WriteObject->SetFloatStat(*Id, Percent);

					FOnlineAchievementsWriteRef WriteObjectRef = WriteObject.ToSharedRef();
					Achievements->WriteAchievements(*UserId, WriteObjectRef);
				}
				else
				{
					UE_LOG(LogOnline, Warning, TEXT("No valid achievement interface or another write is in progress."));
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("No valid User ID for this controller."));
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
		}
		//}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No valid local player found."));
	}
}

void ALPlayerController::OnToggleInGameMenu()
{
	// need to prevent a situation where all players may halt the game at the same time
	// on the same frame
	UWorld* GameWorld = GEngine->GameViewport->GetWorld();

	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
	{
		ALPlayerController* Controller = Cast<ALPlayerController>(*It);
		if (Controller && Controller->IsPaused())
		{
			return;
		}
	}

	// if no one's paused, pause
	//if (ShooterIngameMenu.IsValid())
	//{
	//	ShooterIngameMenu->ToggleGameMenu();
	//}
}

void ALPlayerController::OnConditionalCloseScoreboard()
{

}

void ALPlayerController::OnToggleScoreboard()
{

}

void ALPlayerController::OnShowScoreboard()
{

}

void ALPlayerController::OnHideScoreboard()
{

}

bool ALPlayerController::IsGameMenuVisible() const
{
	bool bResult = false;
	/*if (LIngameMenu.IsValid())
	{
		bResult = LIngameMenu->GetIsGameMenuUp();
	}*/

	return bResult;
}

void ALPlayerController::SetInfiniteAmmo(bool bEnable)
{
	bInfiniteAmmo = bEnable;
}

void ALPlayerController::SetInfiniteClip(bool bEnable)
{
	bInfiniteClip = bEnable;
}

void ALPlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

void ALPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

void ALPlayerController::ClientGameStarted_Implementation()
{
	bAllowGameActions = true;

	//Enable controls mode now the game has started
	SetIgnoreMoveInput(false);

	//ALHUD* LHUD = GetLHUD();
	//if (LHUD)
	//{
	//	LHUD->SetMatchState(ELMatchState::Playing);
	//	LHUD->ShowScoreboard(false);
	//}
	bGameEndedFrame = false;

	QueryAchievements();

	// Send round start event
	const auto Events = Online::GetEventsInterface();
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if (LocalPlayer && Events.IsValid())
	{
		auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();
		if (UniqueId.IsValid())
		{
			// Generate a new Session ID
			Events->SetPlayerSessionId(*UniqueId, FGuid::NewGuid());

			FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());

			// Fire session start event for all classes
			FOnlineEventParms Params;
			Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // TO-DO: Determine the game mode
			Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); //Unused
			Params.Add(TEXT("MapName"), FVariantData(MapName));

			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionStart"), Params);

			// TO-DO: Change this to ULGameInstance
			UGameInstance* LGI = GetWorld() != NULL ? GetWorld()->GetGameInstance() : NULL;
			if (LGI)
			{
				FOnlineEventParms MultiplayerParams;
				MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
				MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // TO-DO: Determine the game mode
				MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // TO-DO: abstract the specific meaning of this value across platforms
				MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

				Events->TriggerEvent(*UniqueId, TEXT("MultiplayerSessionStart"), MultiplayerParams);
			}

			bHasSentStartEvents = true;
		}
	}
}

void ALPlayerController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
	{
		return;
	}

	ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
	if (LPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *LPlayerState->SessionName.ToString());
				Sessions->StartSession(LPlayerState->SessionName);
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &ALPlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
	}
}

void ALPlayerController::ClientEndOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
	if (LPlayerState)
	{
		const auto Sessions = Online::GetSessionInterface();
		if (Sessions.IsValid())
		{
			UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *LPlayerState->SessionName.ToString());
			Sessions->EndSession(LPlayerState->SessionName);
		}
	}
}

void ALPlayerController::HandleReturnToMainMenu()
{
	OnHideScoreboard();
	CleanupSessionOnReturnMenu();
}

void ALPlayerController::ClientReturnToMainMenu_Implementation(const FString& ReturnReason)
{
	// TO-DO: Need to implement this after adding the GameInstance and the User Interface modules
	
	bGameEndedFrame = false;
}

void ALPlayerController::CleanupSessionOnReturnMenu()
{
	// TO-DO: Need to implement this after adding the GameInstance and the User Interface modules
}

void ALPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Disable controls now that the game has ended
	SetIgnoreMoveInput(true);

	bAllowGameActions = false;

	// Make sure we still have a valid view target
	SetViewTarget(GetPawn());

	// TO-DO: Change this to ALHUD
	AHUD* LHUD = GetLHUD();
	if (LHUD)
	{
		//LHUD->SetMatchState(bIsWinner ? ELMatchState::Won : ELMatchState::Lost);
	}

	UpdateSaveFileOnGameEnd(bIsWinner);
	UpdateAchievementsOnGameEnd();
	UpdateLeaderboardsOnGameEnd();

	// Flag that the game has ended
	bGameEndedFrame = true;
}

void ALPlayerController::ClientSendRoundEndEvent_Implementation(bool bIsWinner, int32 ExpendedTimeInSeconds)
{
	const auto Events = Online::GetEventsInterface();
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if (bHasSentStartEvents && LocalPlayer != nullptr && Events.IsValid())
	{
		auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();
		if (UniqueId.IsValid())
		{
			FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());
			ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
			int32 PlayerScore = LPlayerState ? LPlayerState->GetScore() : 0;

			// Fire Session End Events for all cases
			FOnlineEventParms Params;
			Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // TO-DO: determine the game mode
			Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
			Params.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); // unused
			Params.Add(TEXT("PlayerScore"), FVariantData((int32)PlayerScore));
			Params.Add(TEXT("PlayerWon"), FVariantData((bool)bIsWinner));
			Params.Add(TEXT("MapName"), FVariantData(MapName));
			Params.Add(TEXT("MapNameString"), FVariantData(MapName)); // TO-DO: Workaround for a bug in the back end service

			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionEnd"), Params);

			// TO-DO: Change this to ULGameInstance
			UGameInstance* SGI = GetWorld() != NULL ? GetWorld()->GetGameInstance() : NULL;
			if (SGI) // TO-DO: Test for IsOnline here
			{
				FOnlineEventParms MultiplayerParams;

				ALGameState* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<ALGameState>() : NULL;
				if (ensure(MyGameState != nullptr))
				{
					MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
					MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // TO-DO: Determine gameplay mode
					MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // abstract the specific meaning of this value across multiple platforms
					MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
					MultiplayerParams.Add(TEXT("TimeInSeconds"), FVariantData((float)ExpendedTimeInSeconds));
					MultiplayerParams.Add(TEXT("ExitStatusId"), FVariantData((int32)0)); //unused

					Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundEnd"), MultiplayerParams);
				}
			}

		}

		bHasSentStartEvents = false;
	}
}

void ALPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
{
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);

	// If we have a pawn we need to determine if we should show/hide the weapon
	ALabyrinthCharacter* MyPawn = Cast<ALabyrinthCharacter>(GetPawn());
	ALWeapon* MyWeapon = MyPawn ? MyPawn->CurrentWeapon : NULL;
	if (MyWeapon)
	{
		if (bInCinematicMode && bHidePlayer)
		{
			MyWeapon->SetActorHiddenInGame(true);
		}
		else if (!bCinematicMode)
		{
			MyWeapon->SetActorHiddenInGame(false);
		}
	}
}

bool ALPlayerController::IsMoveInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsMoveInputIgnored();
	}
}

bool ALPlayerController::IsLookInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsLookInputIgnored();
	}
}

void ALPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	//TO-DO: Add the ULPersistentUser class
}

void ALPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALPlayerController, bInfiniteAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ALPlayerController, bInfiniteClip, COND_OwnerOnly);
}

void ALPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

bool ALPlayerController::ServerSuicide_Validate()
{
	return true;
}

void ALPlayerController::ServerSuicide_Implementation()
{
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		ALabyrinthCharacter* MyPawn = Cast<ALabyrinthCharacter>(GetPawn());
		if (MyPawn)
		{
			// TO-DO: Add this feature in
			//MyPawn->Suicide();
		}
	}
}

bool ALPlayerController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

bool ALPlayerController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

bool ALPlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

bool ALPlayerController::HasGodMode() const
{
	return bGodMode;
}

bool ALPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

void ALPlayerController::ToggleChatWindow()
{
	// TO-DO: Need to add this in the HUD
}

void ALPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifetime)
{
	// TO-DO: Need to add the HUD
}

void ALPlayerController::Say(const FString& Msg)
{
	ServerSay(Msg.Left(128));
}

bool ALPlayerController::ServerSay_Validate(const FString& Msg)
{
	return true;
}

void ALPlayerController::ServerSay_Implementation(const FString& Msg)
{
	GetWorld()->GetAuthGameMode()->Broadcast(this, Msg, ServerSayString);
}

AHUD* ALPlayerController::GetLHUD() const
{
	return GetHUD();
}

//ULPersistentUser* ALPlayerController::GetPersistentUser() const
//{
//	ULLocalPlayer* const LLP = Cast<ULLocalPlayer>(Player);
//	return LLP ? LLP->GetPersistentUser() : nullptr;
//}

bool ALPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /* = FCanUnpause()*/)
{
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

	// Update rich presence
	const auto PresenceInterface = Online::GetPresenceInterface();
	const auto Events = Online::GetEventsInterface();
	const auto LocalPlayer = Cast<ULocalPlayer>(Player);
	TSharedPtr<FUniqueNetId> UserId = LocalPlayer ? LocalPlayer->GetCachedUniqueNetId() : nullptr;

	if (PresenceInterface.IsValid() && UserId.IsValid())
	{
		FOnlineUserPresenceStatus PresenceStatus;
		if (Result && bPause)
		{
			PresenceStatus.Properties.Add(DefaultPresenceKey, FString("Paused"));
		}
		else
		{
			PresenceStatus.Properties.Add(DefaultPresenceKey, FString("InGame"));
		}

		PresenceInterface->SetPresence(*UserId, PresenceStatus);
	}

	// Don't send pause params to online games
	if (GetNetMode() == NM_Standalone && Events.IsValid() && PlayerState->UniqueId.IsValid())
	{
		FOnlineEventParms Params;
		Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));
		Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0));
		if (Result && bPause)
		{
			Events->TriggerEvent(*PlayerState->UniqueId, TEXT("PlaySessionPause"), Params);
		}
		else
		{
			Events->TriggerEvent(*PlayerState->UniqueId, TEXT("PlaySessionResume"), Params);
		}
	}

	return Result;

}

void ALPlayerController::ShowInGameMenu()
{
	// TO-DO: Add the in game menu class
}

void ALPlayerController::UpdateAchievementsOnGameEnd()
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
		if (LPlayerState)
		{
			// TO-DO:  Add the persistent user class to add stuff here
		}
	}
}

void ALPlayerController::UpdateLeaderboardsOnGameEnd()
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
				if (UserId.IsValid())
				{
					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
					if (Leaderboards.IsValid())
					{
						ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
						if (LPlayerState)
						{
							// TO-DO: Add the LLeaderboards class
							//FLAllTimeMatchResultsWrite WriteObject;
						
							//WriteObject.SetIntStat(LEADERBOARD_STAT_SCORE, LPlayerState->GetKills());
							//WriteObject.SetIntStat(LEADERBOARD_STAT_KILLS, LPlayerState->GetKills());
							//WriteObject.SetIntStat(LEADERBOARD_STAT_DEATHS, LPlayerState->GetDeaths());
							//WriteObject.SetIntStat(LEADERBOARD_STAT_MATCHESPLAYED, 1);

							//// the call will copy the user id and write object to its own memory
							//Leaderboards->WriteLeaderboards(LPlayerState->SessionName, *UserId, WriteObject);
						}
					}
				}
			}
		}
	}
}

void ALPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{
	ALPlayerState* LPlayerState = Cast<ALPlayerState>(PlayerState);
	if (LPlayerState)
	{
		// TO-DO: Add the PersistentUser Class
	}
}

void ALPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	if (GetWorld())
	{
		// TO-DO: Add LGameViewportClient and LHUD classes
	}
}