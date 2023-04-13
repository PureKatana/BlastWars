// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "BlastWars/HUD/BlasterHUD.h"
#include "BlastWars/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "TimerManager.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = !BlasterHUD ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercentage = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = !BlasterHUD ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmount)
	{
		FString ScoreAmountText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreAmountText));
	}
}

void ABlasterPlayerController::SetHUDDeaths(float Deaths)
{
	BlasterHUD = !BlasterHUD ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DeathsAmount)
	{
		FString DeathsAmountText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Deaths));
		BlasterHUD->CharacterOverlay->DeathsAmount->SetText(FText::FromString(DeathsAmountText));
	}
}

void ABlasterPlayerController::SetHUDEliminationText(FString InText)
{
	BlasterHUD = !BlasterHUD ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->EliminationText)
	{
		if (InText.IsEmpty())
		{
			FString TextToDisplay = FString::Printf(TEXT("You eliminated yourself"));
			BlasterHUD->CharacterOverlay->EliminationText->SetText(FText::FromString(TextToDisplay));
		}
		else
		{
			FString TextToDisplay = FString::Printf(TEXT("%s \r eliminated you"), *InText);
			BlasterHUD->CharacterOverlay->EliminationText->SetText(FText::FromString(TextToDisplay));
		}
		BlasterHUD->CharacterOverlay->EliminationText->SetVisibility(ESlateVisibility::Visible);
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
		GetWorldTimerManager().SetTimer(HideTimer, this, &ABlasterPlayerController::HideEliminatedText, BlasterCharacter->GetEliminatedDelay());

	}
}

void ABlasterPlayerController::HideEliminatedText()
{
	BlasterHUD = !BlasterHUD ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->EliminationText)
	{
		BlasterHUD->CharacterOverlay->EliminationText->SetText(FText());
		BlasterHUD->CharacterOverlay->EliminationText->SetVisibility(ESlateVisibility::Hidden);
	}
}
