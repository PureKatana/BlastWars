// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "BlastWars/Weapon/Weapon.h"
#include "BlastWars/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterAnimInstance.h"
#include "BlastWars/BlastWars.h"
#include "BlastWars/PlayerController/BlasterPlayerController.h"
#include "BlastWars/GameMode/BlastWarsGameMode.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BlastWars/PlayerState/BlasterPlayerState.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Spawn Collision
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Set the SpringArm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Set the Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Set Character Orientation
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Set the overhead widget
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	// Set the combat component
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	// Set the crouching bool to true from the character movement
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Set the capsule and mesh to not collide with the camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_Mesh);

	// Set rotation rate
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	// Set the default enum of TurningInPlace
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	// Set NetUpdateFrequency to standard multiplayer shooter games
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	// Set the Dissolve Timeline

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUDHealth();
	UpdateEliminatedText();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Makes sure that it's only autonomous proxy or server and locally controlled. WE can use > since enum in c++ are int and the order matters
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		// Makes sure that ReplicatedMovement is called almost regularly even if we're not moving.
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	//If the character is close to the camera
	HideCamera();

	// Poll all relevant classes and initialize HUD
	PollInitialize();
}

void ABlasterCharacter::PollInitialize()
{
	if (!BlasterPlayerState)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDeaths(0.f);
		}
	}
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind Actions
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireReleased);

	//Bind Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Only replicates to the owner of this pawn
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);

	DOREPLIFETIME(ABlasterCharacter, Health);
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicateMovement();
	SimulatedProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = !BlasterPlayerController ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	return Velocity.Size();
}

void ABlasterCharacter::UpdateEliminatedText()
{
	BlasterPlayerController = !BlasterPlayerController ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->HideEliminatedText();
	}
}

void ABlasterCharacter::Eliminated(ABlasterPlayerController* AttackController)
{
	if (IsWeaponEquipped())
	{
		Combat->EquippedWeapon->Dropped();
	}
	FString AttackerName;
	ABlasterPlayerController* AttackerBlasterController = AttackController;
	if (AttackerBlasterController)
	{
		ABlasterPlayerState* AttackerPlayerState = Cast<ABlasterPlayerState>(AttackerBlasterController->PlayerState);
		if (AttackerPlayerState)
		{
			AttackerName = AttackerPlayerState->GetPlayerName();
		}
	}
	MulticastEliminated(AttackerName);
	GetWorldTimerManager().SetTimer(EliminatedTimer, this, &ABlasterCharacter::EliminatedTimerFinished, EliminatedDelay);
}

void ABlasterCharacter::MulticastEliminated_Implementation(const FString& AttackerName)
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bEliminated = true;
	PlayDeathMontage();

	// Dissolve Effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 100.f);
	}
	StartDissolve();

	// Disable Character Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (BlasterPlayerController)
	{
		DisableInput(BlasterPlayerController);
		BlasterPlayerController->SetHUDEliminationText(AttackerName);
	}

	//Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Death Effect
	if (DeathParticles)
	{
		FName AttachPointName;
		UNiagaraFunctionLibrary::SpawnSystemAttached(DeathParticles, GetMesh(), AttachPointName, GetActorLocation(), GetActorRotation(), EAttachLocation::SnapToTarget, true);
	}
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

void ABlasterCharacter::EliminatedTimerFinished()
{
	ABlastWarsGameMode* BlastWarsGameMode = GetWorld()->GetAuthGameMode<ABlastWarsGameMode>();
	if (BlastWarsGameMode)
	{
		BlastWarsGameMode->RequestRespawn(this, Controller);
		//BlasterPlayerController = !BlasterPlayerController ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			BlasterPlayerController->HideEliminatedText();
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (!Combat || !Combat->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	int32 Selection = FMath::RandRange(0, DeathMontages.Num() - 1);
	if (AnimInstance && DeathMontages[Selection])
	{
		AnimInstance->Montage_Play(DeathMontages[Selection]);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (!Combat || !Combat->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("Front");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		ABlastWarsGameMode* BlastWarsGameMode = GetWorld()->GetAuthGameMode<ABlastWarsGameMode>();

		if (BlastWarsGameMode)
		{
			BlasterPlayerController = !BlasterPlayerController ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlastWarsGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f) 
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::SimulatedProxiesTurn()
{
	if (!Combat || !Combat->EquippedWeapon) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	// Calculate the difference of the rotation from last frame
	ProxyRotLastFrame = ProxyRot;
	ProxyRot = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRot, ProxyRotLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipPressed()
{
	if (Combat)
	{
		//Equip weapon if you are the server
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		//Equip weapon if you are a client by sending a RPC (Remote Procedural Call) that executes this code to another machine
		else
		{
			ServerEquipPressed();
		}
	}
}

void ABlasterCharacter::ServerEquipPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}

void ABlasterCharacter::AimPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	// Checks if the character has a weapon equipped, if not exit out
	if (!IsWeaponEquipped()) return;

	float Speed = CalculateSpeed();
	bool bIsFalling = GetCharacterMovement()->IsFalling();

	// Checks if the character is standing still and not jumping
	if (Speed == 0.f && !bIsFalling)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	// Checks if the character is running or jumping
	if (Speed > 0.f || bIsFalling)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();

}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	// Corrects the compression on the pitch done by the engine to send accross the network
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(260.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	Super::Jump();
}

void ABlasterCharacter::FirePressed()
{
	if (IsWeaponEquipped())
	{
		Combat->FirePressed(true);
	}
}

void ABlasterCharacter::FireReleased()
{
	if (IsWeaponEquipped())
	{
		Combat->FirePressed(false);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 6.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/*OnRep can have parameter input of only the type being replicated. What's passed in is the last value before replication. */
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	//OverlappingWeapon will be null when we end overlapping and LastWeapon will be set to the weapon we stopped overlapping with and vice versa.
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	// Check if the server host is controlling the character
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABlasterCharacter::HideCamera()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (IsWeaponEquipped() && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (IsWeaponEquipped() && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (!Combat) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (!Combat) return FVector();
	return Combat->HitTarget;
}




