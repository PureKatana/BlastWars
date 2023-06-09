// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastWarsGameMode.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "BlastWars/PlayerState/BlasterPlayerState.h"
#include "BlastWars/GameState/BlastWarsGameState.h"

namespace MatchState 
{
	const FName Cooldown = FName("Cooldown");
}

ABlastWarsGameMode::ABlastWarsGameMode()
{
	bDelayedStart = true;
}

void ABlastWarsGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlastWarsGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = WarmupTime + MatchTime + CooldownTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlastWarsGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// Loops through all player controllers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState, bTeamMatch);
		}
	}
}

float ABlastWarsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void ABlastWarsGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlastWarsGameState* BlastWarsGameState = GetGameState<ABlastWarsGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlastWarsGameState)
	{
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : BlastWarsGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		BlastWarsGameState->UpdateTopScore(AttackerPlayerState);
		if (BlastWarsGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlastWarsGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDeaths(1.f);
	}
	
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Eliminated(AttackerController, false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElimination(AttackerPlayerState, VictimPlayerState);
		}
	}
	
}

void ABlastWarsGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController)
{
	if (EliminatedCharacter)
	{
		// Character unpossess
		EliminatedCharacter->Reset();

		EliminatedCharacter->Destroy();
	}

	if (EliminatedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(EliminatedController, PlayerStarts[Selection]);
	}
}

void ABlastWarsGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (!PlayerLeaving) return;
	ABlastWarsGameState* BlastWarsGameState = GetGameState<ABlastWarsGameState>();
	if (BlastWarsGameState && BlastWarsGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlastWarsGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Eliminated(nullptr, true);
	}
}
