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
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void HideEliminatedText();

	virtual void OnPossess(APawn* InPawn) override;
	// Synced with server world clock
	virtual float GetServerTime();
	// Sync with server clock as soon as possible
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void HandleMatchHasStarted();
protected:

	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInitialize();
	virtual void Tick(float DeltaTime) override;

	// Sync time between clients and server

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);
	// Difference between Client and Server time
	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	// Makes sure we send another request every frequency to make sure we get the correct server time;
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime);
private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	float HUDDeaths;

};
