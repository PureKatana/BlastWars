// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastWarsGameState.h"
#include "Net/UnrealNetwork.h"
#include "BlastWars/PlayerState/BlasterPlayerState.h"

void ABlastWarsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlastWarsGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlastWarsGameState, RedTeamScore);
	DOREPLIFETIME(ABlastWarsGameState, BlueTeamScore);
}

void ABlastWarsGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.IsEmpty())
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if(ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlastWarsGameState::OnRep_RedTeamScore()
{

}

void ABlastWarsGameState::OnRep_BlueTeamScore()
{

}
