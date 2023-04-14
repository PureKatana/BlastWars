// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlastWarsGameMode.generated.h"

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
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	float LevelStartingTime = 0.f;
protected:

	virtual void BeginPlay() override;

private:

	float CountdownTime = 0.f;
};
