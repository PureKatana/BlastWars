// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"
#include "BlastWars/BlasterComponents/LagCompensationComponent.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to the number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadshotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				const bool bHeadshot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadshot)
				{
					if (HeadshotHitMap.Contains(BlasterCharacter)) HeadshotHitMap[BlasterCharacter]++;
					else HeadshotHitMap.Emplace(BlasterCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
					else HitMap.Emplace(BlasterCharacter, 1);
				}
				
				if (BlasterCharacter->GetShield() > 0.f)
				{
					if (ShieldHitParticles)
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShieldHitParticles, FireHit.ImpactPoint);
					}
					if (ShieldHitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, ShieldHitSound, FireHit.ImpactPoint, 0.5f);
					}
				}
				else
				{
					if (HitParticles)
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitParticles, FireHit.ImpactPoint);
					}
					if (HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, 0.5f);
					}
				}
			}
			else
			{
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
				}
				if (ImpactSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, FireHit.ImpactPoint, 0.5f, FMath::FRandRange(-0.5f, 0.5f));
				}
			}
		}
		TArray<ABlasterCharacter*> HitCharacters;
		// Maps character hit to total damage
		TMap<ABlasterCharacter*, float> DamageMap;
		// Calculate body shot damage by multiplying times hit x Damage --> stores in the DamageMap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				HitCharacters.AddUnique(HitPair.Key);
			}
		}
		// Calculate head shot damage by multiplying times hit x HeadshotDamage --> stores in the DamageMap
		for (auto HeadshotHitPair : HeadshotHitMap)
		{
			if (HeadshotHitPair.Key)
			{
				if (DamageMap.Contains(HeadshotHitPair.Key)) DamageMap[HeadshotHitPair.Key] += HeadshotHitPair.Value * HeadshotDamage;
				else DamageMap.Emplace(HeadshotHitPair.Key, HeadshotHitPair.Value * HeadshotDamage);
				HitCharacters.AddUnique(HeadshotHitPair.Key);
			}
		}
		// Loops through DamageMap to get the total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthorityDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthorityDamage)
				{
					// Apply damage with the damage calculated above
					UGameplayStatics::ApplyDamage(DamagePair.Key,DamagePair.Value, InstigatorController, this, UDamageType::StaticClass());
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = !BlasterOwnerCharacter ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = !BlasterOwnerController ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;

			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(HitCharacters, Start, HitTargets, BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (!MuzzleFlashSocket) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector Start = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - Start).GetSafeNormal();
	const FVector SphereCenter = Start + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfShots; i++)
	{
		const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLocation = SphereCenter + RandomVector;
		FVector ToEndLocation = EndLocation - Start;
		ToEndLocation = FVector(Start + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size());

		HitTargets.Add(ToEndLocation);
	}
}


