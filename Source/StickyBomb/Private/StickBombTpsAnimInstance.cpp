// Fill out your copyright notice in the Description page of Project Settings.


#include "StickBombTpsAnimInstance.h"

#include "StickyBombCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UStickBombTpsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(const auto Character = Cast<AStickyBombCharacter>(GetSkelMeshComponent()->GetOwner()))
	{
		bIsMoving = Character->GetMovementComponent()->Velocity.SizeSquared() > 1.0f;
	}
}
