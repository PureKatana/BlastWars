// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Deaths);
	DOREPLIFETIME(ABlasterPlayerState, Team);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	// In server
	SetScore(GetScore() + ScoreAmount);
	Character = !Character ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = !Controller ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
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
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDeaths(float DeathsAmount)
{
	Deaths += DeathsAmount;
	Character = !Character ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = !Controller ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDeaths(Deaths);
		}
	}
}

void ABlasterPlayerState::OnRep_Deaths()
{
	Character = !Character ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = !Controller ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDeaths(Deaths);
		}
	}
}

