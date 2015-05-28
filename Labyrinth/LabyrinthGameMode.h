// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "LabyrinthGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALabyrinthGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALabyrinthGameMode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<class APawn> BotPawnClass;

	UFUNCTION(exec)
	void SetAllowBots(bool bInAllowBots, int32 InMaxBots = 8);

	/** Initialize the game. Called before the actors PreInitializeComponents */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or reject a player attempting to join the server. Fails the login if you set the ErrorMessage to a non-empty string */
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<class FUniqueNetId>& UniqueNetId, FString& ErrorMessage) override;

	/** Starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Select best spawn point for player */
	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/** Always pick new random spawn point */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** returns the default pawn class for the given controller */
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	/** Prevents friendly fire */
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/** Notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** Can players damage each other? */
	virtual bool CanDealDamage(class ALPlayerState* DamageInstigator, class ALPlayerState* DamagedPlayer) const;

	/** The Cheat Manager */
	virtual bool AllowCheats(APlayerController* P) override;

	/** update remaining time. for timed matches(?) */
	virtual void DefaultTimer() override;

	/** called before MatchStart */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;

	/** hide the onscreen hud and restart the map */
	virtual void RestartGame() override;

	/** Create an AI Controller for the bots */
	void CreateBotControllers();

	/** Create a Bot */
	//ALAIController* CreateBot(int32 BotNum);

protected:

	/** delay between first player login and starting match */
	UPROPERTY(config)
	int32 WarmupTime;

	/** match duration */
	UPROPERTY(config)
	int32 RoundTime;

	UPROPERTY(config)
	int32 TimeBetweenMatches;

	/** score for kill */
	UPROPERTY(config)
	int32 KillScore;
	
	/** score for death */
	UPROPERTY(config)
	int32 DeathScore;

	/** scale for self instigated damage */
	UPROPERTY(config)
	float DamageSelfScale;

	/** Max number of bots */
	UPROPERTY(config)
	int32 MaxBots;

	//UPROPERTY()
	//TArray<ALAIController*> BotControllers;

	bool bNeedsBotCreation;

	bool bAllowBots;

	/** Spawning all bots for this game */
	void StartBots();

	/** intialization after bot creation */
	//virtual void InitBot(ALAIController* AIC, int32 BotNum);

	/** check who won */
	virtual void DetermineMatchWinner();

	/** check if playerstate is a winner */
	virtual bool IsWinner(class ALPlayerState* PlayerState) const;

	/** check if player can use spawn point */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check if player should use spawn point */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;

	/** Returns the game session class */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

public:
	
	/** finish the match and lock the players */
	UFUNCTION(exec)
	void FinishMatch();

	/** Finish match and bump everyone to main menu. Only GameInstance should call this function */
	void RequestFinishAndExitToMainMenu();

	/** get the name of the bots count option used in the server travel URL */
	static FString GetBotsCountOptionName();

	UPROPERTY()
	TArray<class ALPickupBase*> LevelPickups;
};
