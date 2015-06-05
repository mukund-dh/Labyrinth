// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "LabyrinthGameMode.h"
#include "Public/LPlayerController.h"
#include "Public/LPlayerState.h"
#include "Public/LHUD.h"
#include "Public/LGameState.h"
#include "Public/GameplayBase/LGameSession.h"


ALabyrinthGameMode::ALabyrinthGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the default pawn class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/MainChar"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ALHUD::StaticClass();

	PlayerControllerClass = ALPlayerController::StaticClass();
	PlayerStateClass = ALPlayerState::StaticClass();
	GameStateClass = ALGameState::StaticClass();

	MinRespawnDelay = 5.0f;

	bAllowBots = false;
	bNeedsBotCreation = false;
	bUseSeamlessTravel = true;
}

FString ALabyrinthGameMode::GetBotsCountOptionName()
{
	return FString(TEXT("Bots"));
}

void ALabyrinthGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	const int32 BotsCountOptionValue = GetIntOption(Options, GetBotsCountOptionName(), 0);
	SetAllowBots(BotsCountOptionValue > 0 ? true : false, BotsCountOptionValue);
	Super::InitGame(MapName, Options, ErrorMessage);

	// TO-DO: Create a ULabyrinthGameInstance class
	/*const UGameInstance* GI = GetGameInstance();
	if (GI && Cast<ULabyrinthGameInstance>(GI)->GetIsOnline())
	{
		bPauseable = false;
	}*/
}

void ALabyrinthGameMode::SetAllowBots(bool bInAllowBots, int32 InMaxBots)
{
	bAllowBots = bInAllowBots;
	MaxBots = InMaxBots;
}

TSubclassOf<AGameSession> ALabyrinthGameMode::GetGameSessionClass() const
{
	// TO-DO: Change this to ALabyrinthGameSession
	return ALGameSession::StaticClass();
}

void ALabyrinthGameMode::DefaultTimer()
{
	Super::DefaultTimer();

	// don't update timers for play in editor mode, it's not a real match
	if (GetWorld()->IsPlayInEditor())
	{
		// start the match if necessary
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		return;
	}

	ALGameState* const MyGameState = Cast<ALGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bIsTimerPaused)
	{
		MyGameState->RemainingTime--;
		if (MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				RestartGame();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();

				// Send end round events
				ALGameState* const MyGameState = Cast<ALGameState>(GameState);

				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; It++)
				{
					ALPlayerController* PlayerController = Cast<ALPlayerController>(*It);
					if (PlayerController && MyGameState)
					{
						ALPlayerState* PlayerState = Cast<ALPlayerState>((*It)->PlayerState);
						const bool bIsWinner = IsWinner(PlayerState);

						// TO-DO: Add this to the Player controller
						//PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
					}
				}
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
			}
		}
	}
}

void ALabyrinthGameMode::HandleMatchIsWaitingToStart()
{
	if (bNeedsBotCreation)
	{
		CreateBotControllers();
		bNeedsBotCreation = false;
	}

	if (bDelayedStart)
	{
		// start warmup if necessary
		ALGameState* const MyGameState = Cast<ALGameState>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
			if (bWantsMatchWarmup && WarmupTime > 0)
			{
				MyGameState->RemainingTime = WarmupTime;
			} 
			else
			{
				MyGameState->RemainingTime = 0.f;
			}
		}
	}
}

void ALabyrinthGameMode::HandleMatchHasStarted()
{
	bNeedsBotCreation = true;
	Super::HandleMatchHasStarted();

	ALGameState* const MyGameState = Cast<ALGameState>(GameState);
	MyGameState->RemainingTime = RoundTime;
	StartBots();

	// notify players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ALPlayerController* PC = Cast<ALPlayerController>(*It);
		if (PC)
		{
			// TO-Do: Add this to ALPlayerController
			//PC->ClientGameStarted();
		}
	}
}

void ALabyrinthGameMode::FinishMatch()
{
	ALGameState* const MyGameState = Cast<ALGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ALPlayerState* PlayerState = Cast<ALPlayerState>((*It)->PlayerState);
			if (PlayerState)
			{
				const bool bIsWinner = IsWinner(PlayerState);

				(*It)->GameHasEnded(NULL, bIsWinner);
			}
		}
		// lock all pawns
		// pawns are not marked as keep for seamless travel, so we will create new ones
		// instead of turning these back on
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			(*It)->TurnOff();
		}

		// Setup to restart the match
		MyGameState->RemainingTime = TimeBetweenMatches;
	}
}

void ALabyrinthGameMode::RequestFinishAndExitToMainMenu()
{
	FinishMatch();

	// TO-DO: Change this to ULGameInstance
	UGameInstance* const GI = GetGameInstance();
	if (GI)
	{
		//GI->RemoveSplitScreenPlayers;
	}

	ALPlayerController* LocalPrimaryController = nullptr;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ALPlayerController* Controller = Cast<ALPlayerController>(*It);
		if (Controller == NULL)
		{
			continue;
		}

		if (!Controller->IsLocalController())
		{
			const FString RemoteReturnReason = NSLOCTEXT("Network Errors", "HostHasLeft", "Host has left the game.").ToString();
			Controller->ClientReturnToMainMenu(RemoteReturnReason);
		}
		else
		{
			LocalPrimaryController = Controller;
		}
	}

	// GameInstance should call this from an EndState. So call the PC function that performs cleanup
	// not the one that sets GI state
	if (LocalPrimaryController != NULL)
	{
		// TO-DO: Add this to the player Controller.
		//LocalPrimaryController->HandleReturnToMainMenu();
	}
}

void ALabyrinthGameMode::DetermineMatchWinner()
{
	// DO nothing
}

bool ALabyrinthGameMode::IsWinner(class ALPlayerState* PlayerSate) const
{
	return false;
}

void ALabyrinthGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	ALGameState* const MyGameState = Cast<ALGameState>(GameState);

	const bool bMatchIsOver = MyGameState && MyGameState->HasMatchEnded();
	if (bMatchIsOver)
	{
		ErrorMessage = TEXT("Match is Over!");
	}
	else
	{
		// GameSession can be NULL if the match is over
		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	}
}

void ALabyrinthGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Update the spectator location for client
	ALPlayerController* NewPC = Cast<ALPlayerController>(NewPlayer);
	if (NewPC && NewPC->GetPawn() == NULL)
	{
		// TO-DO: Add this to ALPlayerController
		//NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation);
	}

	// Notify the new plaer if match is already in progress
	if (NewPC && IsMatchInProgress())
	{
		// TO-DO: Add this to the ALPlayerController
		//NewPC->ClientGameStarted();
		//NewPC->ClientStartOnlineGame();
	}
}

void ALabyrinthGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	ALPlayerState* KillerPlayerState = Killer ? Cast<ALPlayerState>(Killer->PlayerState) : NULL;
	ALPlayerState* VictimPlayerState = KilledPlayer ? Cast<ALPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}
}

float ALabyrinthGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	ALabyrinthCharacter* DamagedPawn = Cast<ALabyrinthCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ALPlayerState* DamagedPlayerState = Cast<ALPlayerState>(DamagedPawn->PlayerState);
		ALPlayerState* InstigatorPlayerState = Cast<ALPlayerState>(EventInstigator->PlayerState);

		// disable friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}

		// scale self inflicted damage
		if (InstigatorPlayerState == DamagedPlayerState)
		{
			ActualDamage *= DamageSelfScale;
		}
	}

	return ActualDamage;
}

bool ALabyrinthGameMode::CanDealDamage(class ALPlayerState* DamageInstigator, class ALPlayerState* DamagedPlayer) const
{
	return true;
}

bool ALabyrinthGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

bool ALabyrinthGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

UClass* ALabyrinthGameMode::GetDefaultPawnClassForController(AController* InController)
{
	// TO-DO: Add AI
	/*if (Cast<ALAIController>(InController))
	{
		return BotPawnClass;
	}*/

	return Super::GetDefaultPawnClassForController(InController);
}

AActor* ALabyrinthGameMode::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestSpawn = PlayerStarts[i];
		if (Cast<APlayerStartPIE>(TestSpawn) != NULL)
		{
			// Always prefer the first "Play from here PlayerStar
			// in the PIE mode
			BestStart = TestSpawn;
			break;
		}
		else if (TestSpawn != NULL)
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}

	if (BestStart == NULL)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}

bool ALabyrinthGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	// TO-DO: Change SpawnPoint type to ALTeamStart
	if (SpawnPoint)
	{
		// To-DO: Add AI Controller
		/*ALAIController* AIController = Cast<ALAIController>(Player);
		if (SpawnPoint->bNotForBots && AIController)
		{
			return false;
		}

		if (SpawnPoint->bNotforPlayers && AIController == NULL)
		{
			return false;
		}*/

		return true;
	}

	return false;
}

bool ALabyrinthGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Cast<ACharacter>((*DefaultPawnClass)->GetDefaultObject<ACharacter>());

	// TO-DO: Add AI Controller
	//ALAIController* AIController = Cast<ALAIController>(Player);
	/*if (AIController != nullptr)
	{
		MyPawn = Cast<ACharacter>(BotPawnClass->GetDefaultObject<ACharacter>());
	}*/

	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius());
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// Check if the player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

void ALabyrinthGameMode::CreateBotControllers()
{
	// TO-DO: Add stuff in to create bots
}

void ALabyrinthGameMode::StartBots()
{
	// TO-DO: Add stuff here to Start the bots
}

void ALabyrinthGameMode::RestartGame()
{
	// Hide the scoreboard
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; It++)
	{
		ALPlayerController* PlayerController = Cast<ALPlayerController>(*It);
		if (PlayerController != nullptr)
		{
			// HUD related stuff
		}
	}

	Super::RestartGame();
}