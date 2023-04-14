// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDeaths(float Deaths);
	void SetHUDEliminationText(FString InText);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponType(FText WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void HideEliminatedText();

	virtual void OnPossess(APawn* InPawn) override;

protected:

	virtual void BeginPlay() override;
	void SetHUDTime();
	virtual void Tick(float DeltaTime) override;
private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
	
};
