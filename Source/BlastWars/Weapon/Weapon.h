// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlastWars/Types/WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTWARS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	// Textures for the weapon crosshair

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairBottom;

	// FOV Zoomed while aiming

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	// Automatic Fire

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float FireDelay = 0.15f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bAutomatic = true;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState,VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletShell> BulletShellClass;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	UFUNCTION()
	void OnRep_Ammo();
	void SpendRound();
	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;
	UPROPERTY(EditAnywhere)
	class USoundCue* EmptyShotSoundCue;
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE bool IsEmpty() { return Ammo <= 0; }
	FORCEINLINE USoundCue* GetEmptyShotSoundCue() { return EmptyShotSoundCue; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
};
