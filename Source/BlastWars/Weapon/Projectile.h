// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTWARS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Use with server side rewind
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	// 2 decimal place of precision
	FVector_NetQuantize100 InitialVelocity;
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000.f;

	// Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	// Bullets
	UPROPERTY(EditAnywhere)
	float HeadshotDamage = 40.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* HitParticles;
	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ShieldHitParticles;
	UPROPERTY(EditAnywhere)
	USoundCue* ShieldHitSound;
    UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(EditAnywhere)
	float MinimumDamage = 20.f;
	UPROPERTY(EditAnywhere)
	float DamageInRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float DamageOutRadius = 500.f;

private:

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyOnHitTimer;
	UPROPERTY(EditAnywhere)
	float DestroyOnHitTime = 3.f;

public:	
	

};
