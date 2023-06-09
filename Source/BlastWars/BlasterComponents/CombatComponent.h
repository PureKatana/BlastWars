// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlastWars/HUD/BlasterHUD.h"
#include "BlastWars/Types/WeaponTypes.h"
#include "BlastWars/Types/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTWARS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	//Allows ABlasterCharacter class to have all functions and variables from the Combat Component
	friend class ABlasterCharacter;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void SwapWeapon();
	void Reload();
	UFUNCTION(Blueprintcallable)
	void FinishedReloading();
	UFUNCTION(Blueprintcallable)
	void FinishedSwap();
	UFUNCTION(Blueprintcallable)
	void FinishedSwapAttachWeapons();

	void FirePressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	void PickupGrenades(int32 GrenadeAmount);

	bool bLocallyReloading = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming); 

	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();

	void Fire();
	void FireProjectileWeapon();
	void FireHitscanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();
	int32 AmountToReload();

	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;

	void DropEquippedWeapon();
	void AttachActorToSocket(AActor* ActorToAttach, FName SocketName);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void ShowAttachedGrenade(bool bShowGrenade);

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void EquipFlag(AWeapon* FlagToEquip);

private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	UPROPERTY()
	class ABlasterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
    AWeapon* EquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_EquipFlag)
	AWeapon* Flag;
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;
	bool bAimPressed = false;
	UFUNCTION()
	void OnRep_Aiming();
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	bool bFirePressed;

	//HUD and Crosshairs

	float CrosshairBaseLine = 0.5f;
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	FHUDPackage HUDPackage;
	FVector HitTarget;

	// Aiming and FOV

	// FOV when not aiming (camera's base FOV)
	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
	void InterpFOV(float DeltaTime);

	// Automatic Fire

	FTimerHandle FireTimer;
	bool bCanFire = true;
	void FireTimerFinished();
	void StartFireTimer();

	// Carried Ammo
	bool CanFire();
	
	// Carried ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();
	UPROPERTY(EditAnywhere)
	uint32 MaxCarriedAmmo = 400;

	// Replication doesn't work on TMap
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 40;
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 24;
	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 12;
	UPROPERTY(EditAnywhere)
	int32 StartingGLAmmo = 18;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();
	FText GetDisplayNameWeaponType() const;
	
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 6;
	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 6;
	UFUNCTION()
	void OnRep_Grenades();
	void UpdateHUDGrenades();

	UFUNCTION()
	void OnRep_EquipFlag();

public:	
	
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	bool ShouldSwapWeapon();
		
};
