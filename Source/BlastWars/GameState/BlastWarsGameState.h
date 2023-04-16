// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlastWarsGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API ABlastWarsGameState : public AGameState
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ABlasterPlayerState* ScoringPlayer);
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;

private:
	
	float TopScore = 0.f;

public:

};
