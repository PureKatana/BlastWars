// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastWarsGameMode.generated.h"

namespace MatchState
{
	// Match duration ended, display winner and begin cooldown timer
	extern BLASTWARS_API const FName Cooldown;
}

/**
 * 
 */
UCLASS()
class BLASTWARS_API ABlastWarsGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ABlastWarsGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
	void PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving);
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;
	float LevelStartingTime = 0.f;

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	bool bTeamMatch = false;
protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;

public:

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
