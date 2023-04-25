// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"
#include "BlastWars/GameState/BlastWarsGameState.h"
#include "BlastWars/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABlastWarsGameState* BlastWarsGameState = Cast<ABlastWarsGameState>(UGameplayStatics::GetGameState(this));
	if (BlastWarsGameState)
	{
		ABlasterPlayerState* BPState = NewPlayer->GetPlayerState<ABlasterPlayerState>();
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BlastWarsGameState->BlueTeam.Num() >= BlastWarsGameState->RedTeam.Num())
			{
				BlastWarsGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_Red);
			}
			else
			{
				BlastWarsGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_Blue);
			}
		}
	}
}

void ATeamGameMode::Logout(AController* Exiting)
{
	ABlastWarsGameState* BlastWarsGameState = Cast<ABlastWarsGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Exiting->GetPlayerState<ABlasterPlayerState>();
	if (BlastWarsGameState && BPState)
	{
		if (BlastWarsGameState->RedTeam.Contains(BPState))
		{
			BlastWarsGameState->RedTeam.Remove(BPState);
		}
		if (BlastWarsGameState->BlueTeam.Contains(BPState))
		{
			BlastWarsGameState->BlueTeam.Remove(BPState);
		}
	}
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlastWarsGameState* BlastWarsGameState = Cast<ABlastWarsGameState>(UGameplayStatics::GetGameState(this));
	if (BlastWarsGameState)
	{
		for (auto PlayerState : BlastWarsGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PlayerState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BlastWarsGameState->BlueTeam.Num() >= BlastWarsGameState->RedTeam.Num())
				{
					BlastWarsGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_Red);
				}
				else
				{
					BlastWarsGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_Blue);
				}
			}
		}
	}
}
