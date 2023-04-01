// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
		return;
	}

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	GroundSpeed = Velocity.Size();

	bIsFalling = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	

}
