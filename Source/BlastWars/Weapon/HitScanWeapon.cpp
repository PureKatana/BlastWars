// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "BlastWars/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "BlastWars/Types/WeaponTypes.h"
#include "BlastWars/BlasterComponents/LagCompensationComponent.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
        FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter)
		{
			if (BlasterCharacter->GetShield() > 0.f)
			{
				if (ShieldHitParticles)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShieldHitParticles, FireHit.ImpactPoint);
				}
				if (ShieldHitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, ShieldHitSound, FireHit.ImpactPoint);
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
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
				}
			}
			if (InstigatorController)
			{
				bool bCauseAuthorityDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthorityDamage)
				{
					const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadshotDamage : Damage;
					
					UGameplayStatics::ApplyDamage(BlasterCharacter, DamageToCause, InstigatorController, this, UDamageType::StaticClass());
				}
				if(!HasAuthority() && bUseServerSideRewind)
				{
					BlasterOwnerCharacter = !BlasterOwnerCharacter ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
					BlasterOwnerController = !BlasterOwnerController ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;

					if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
					{
						BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(BlasterCharacter, Start, HitTarget, BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime);
					}
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
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, FireHit.ImpactPoint);
			}
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility);
		FVector EndBeam = End;
		if (OutHit.bBlockingHit)
		{
			EndBeam = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator, true);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), EndBeam);
			}
		}
	}
}


