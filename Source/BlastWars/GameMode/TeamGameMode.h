// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlastWarsGameMode.h"
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API ATeamGameMode : public ABlastWarsGameMode
{
	GENERATED_BODY()
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:

	virtual void HandleMatchHasStarted() override;

	
};
