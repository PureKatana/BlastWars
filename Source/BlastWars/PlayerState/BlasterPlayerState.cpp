// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	// In server
	Score += ScoreAmount;
	Character = !Character ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = !Controller ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = !Character ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = !Controller ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}


