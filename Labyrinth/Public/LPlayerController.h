// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "OnlineStats.h"
#include "LPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALPlayerController : public APlayerController
{
	GENERATED_BODY()

	ALPlayerController(const FObjectInitializer& ObjectInitializer);

public:

	/** Sets the spectator location and rotation */
	UFUNCTION(Reliable, Client)
	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	/** notify player about the started match */
	UFUNCTION(Reliable, Client)
	void ClientGameStarted();

	/** Starts the online game using the session name in the playerstate */
	UFUNCTION(Reliable, Client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the playerstate */
	UFUNCTION(Reliable, Client)
	void ClientEndOnlineGame();

	/** notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	/** Notifies client to send end of round events */
	UFUNCTION(Reliable, Client)
	void ClientSendRoundEndEvent(bool bIsWinner, int32 ExpendedTimeInSeconds);

	/** Used for input simulation from blueprint (for automatic perf tests) */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SimulateInputKey(FKey Key, bool bIsPressed = true);

	/** Sends cheat message */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);
	virtual void ServerCheat_Implementation(const FString& Msg);
	virtual bool ServerCheat_Validate(const FString& Msg);

	/** Overridden Message Implementation */
	virtual void ClientTeamMessage_Implementation(APlayerState* SendPlayerState, const FString& S, FName Type, float MsgLifeTime) override;

	/** Tell the HUD to toggle the chat window */
	void ToggleChatWindow();

	/** Local function "Say a string" */
	UFUNCTION(exec)
	virtual void Say(const FString& Msg);

	/** RPC for clients to talk to the server */
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerSay(const FString& Msg);
	virtual void ServerSay_Implementation(const FString& Msg);
	virtual bool ServerSay_Validate(const FString& Msg);

	/** Notify local client about deaths */
	void OnDeathMessage(class ALPlayerState* KillerPlayerState, class ALPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** Toggle in-game menu handler */
	void OnToggleInGameMenu();

	/** Show in game menu if it's not already showing */
	void ShowInGameMenu();

	/** Hide scoreboard if currently diplayed */
	void OnConditionalCloseScoreboard();

	/** Toggles Scoreboard */
	void OnToggleScoreboard();

	/** shows scoreboard */
	void OnShowScoreboard();

	/** Hides scoreboard */
	void OnHideScoreboard();

	/** Set infinite ammo cheat */
	void SetInfiniteAmmo(bool bEnable);

	/** Set infinite clip cheat */
	void SetInfiniteClip(bool bEnable);

	/** Set Health Regen cheat */
	void SetHealthRegen(bool bEnable);

	/** Set God mode cheat */
	UFUNCTION(exec)
	void SetGodMode(bool bEnable);

	/** Get infinite ammo cheat */
	bool HasInfiniteAmmo() const;

	/** Get infinite clip cheat */
	bool HasInfiniteClip() const;

	/** Get health regen cheat */
	bool HasHealthRegen() const;

	/** Get God mode cheat */
	bool HasGodMode() const;

	/** Check if Gameplay related activities (movement, weapon usage, etc.) are allowed */
	bool IsGameInputAllowed() const;

	/** Is Game Menu currently active */
	bool IsGameMenuVisible() const;

	/** Ends or destroys the game session */
	void CleanupSessionOnReturnMenu();

	/**
	 * Called when the read achievements request from the server is complete.
	 * @param PlayerId The player id who is responsible for this delegate being fired
	 * @param bWasSuccessful true if the server responded successfully to this request
	 */
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);

	// Begin APlayerController Interface
	
	/** Handle Weapon Visibility */
	virtual void SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning) override;

	/** Returns true if movement input is ignored. Overridden to always allow spectators to move */
	virtual bool IsMoveInputIgnored() const;

	/** Returns true if look input is ignored. Overridden to always allow spectators to look around */
	virtual bool IsLookInputIgnored() const;

	/** Initialize the input system from the player settings */
	virtual void InitInputSystem() override;

	/** Pause game (?) */
	virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;

	// End APlayerController Interface

	// Begin ALPlayerController specific interface

	/** Reads achievements to precache them before first use */
	void QueryAchievements();

	/** 
	 * Writes a single achievement (unless another write is in progress) 
	 *
	 * @param Id acheivement Id (string)
	 * @param Percent number 1 to 100
	 */
	void UpdateAchievementProgress(const FString& Id, float Percent);

	/** 
	 * Returns a pointer to the shooter game HUD
	 * TO-DO: Change this to ALHUD 
	 */
	AHUD* GetLHUD() const;

	/** 
	 * Returns the persistent user record associated with this player
	 * or NULL if there isn't any
	 * TO-DO: Add this class.
	 */
	//class ULPersistentUser* GetPersistentUser() const;

	/** Informs that player fragged someone */
	void OnKill();

	/** Cleans up any resources necessary to return to the Main Menu. Does not modify GameInstance state */
	virtual void HandleReturnToMainMenu()

	/** Associate new player UPlayer with this PlayerController */
	virtual void SetPlayer(UPlayer* Player);

	// End ALPlayerController specific interface

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

protected:

	/** Infinite ammo cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteAmmo : 1;

	/** Infinite clip cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteClip : 1;

	/** Health Regen cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bHealthRegen : 1;

	/** God mode cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bGodMode : 1;

	/** If set, gameplay related actions (movement, weapon usage, etc.) */
	uint8 bAllowGameActions : 1;

	/** true for the frst frame after the game has ended */
	uint8 bGameEndedFrame : 1;

	/** stores pawn location at last player death, used where player scores a kill after they died */
	FVector LastDeathLocation;

	/** TO-DO: Add in-game menu */
	//TSharedPtr<class FLInGameMenu> LInGameMenu;

	/** Achievements write object */
	FOnlineAchievementsWritePtr WriteObject;

	/** try to find a spot for the death cam */
	bool FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation);

	// Begin AActor Interface

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	// End AActor interface

	// Begin AController interface

	/** transition to dead state */
	virtual void FailedToSpawnPawn() override;

	/** Update Camera when pawn dies */
	virtual void PawnPendingDestroy(APawn* P) override;

	// End AController interface

	// Begin APlayerController interface

	/** Respawn after dying */
	virtual void UnFreeze() override;

	/** sets up input */
	virtual void SetupInputComponent() override;

	/**
	 * Called from game info upon end of the game, used to transition to a proper state
	 */
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;

	/** Return the client to the main menu gracefully. Only sets the GI state */
	void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
	virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSuicide();
	virtual void ServerSuicide_Implementation();
	virtual bool ServerSuicide_Validate();

	/** Updates achievements based on the PersistentUse stats at the end of a round */
	void UpdateAchievementsOnGameEnd();

	/** Updates leaderboard stats at the end of a round */
	void UpdateLeaderboardsOnGameEnd();

	/** Updates the save file at the end of a game */
	void UpdateSaveFileOnGameEnd(bool bIsWinner);

	// End APlayerController interface

	FName ServerSayString;

	// Timer used for updating friends in the player tick
	float LFriendUpdateTimer;

	// For tracking whether or not to send the end event
	bool bHasSentStartEvents;

private:

	/** Handle for efficient management of ClientStartOnlineGame */
	FTimerHandle TimerHandle_ClientStartOnlineGame;
};
