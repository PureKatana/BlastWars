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

	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);
};
