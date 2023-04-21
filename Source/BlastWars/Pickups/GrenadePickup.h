// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GrenadePickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API AGrenadePickup : public APickup
{
	GENERATED_BODY()

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:

	int32 GrenadeAmount = 3;
};
