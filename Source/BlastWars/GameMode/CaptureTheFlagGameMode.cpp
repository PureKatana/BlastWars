// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagGameMode.h"
#include "BlastWars/Weapon/Flag.h"
#include "BlastWars/CaptureTheFlag/FlagZone.h"
#include "BlastWars/GameState/BlastWarsGameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlastWarsGameMode::PlayerEliminated(EliminatedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	ABlastWarsGameState* BlastWarsGameState = Cast<ABlastWarsGameState>(GameState);
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	if (BlastWarsGameState)
	{
		if (Zone->Team == ETeam::ET_Blue)
		{
			BlastWarsGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_Red)
		{
			BlastWarsGameState->RedTeamScores();
		}
	}
}
