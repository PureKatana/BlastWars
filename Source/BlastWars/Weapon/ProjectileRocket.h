// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileRocket();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	UPROPERTY(EditAnywhere)
	float MinimumDamage = 20.f;

	UPROPERTY(EditAnywhere)
	float DamageInRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOutRadius = 500.f;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
};
