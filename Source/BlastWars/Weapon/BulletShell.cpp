// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABulletShell::ABulletShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BulletShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	SetRootComponent(BulletShellMesh);
	BulletShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BulletShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// Enable physics and gravity
	BulletShellMesh->SetSimulatePhysics(true);
	BulletShellMesh->SetEnableGravity(true);
	BulletShellMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 8.f;
}

// Called when the game starts or when spawned
void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
	BulletShellMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	BulletShellMesh->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound && !bPlaySoundOnce)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		bPlaySoundOnce = true;
		SetLifeSpan(3.f);
	}
}


