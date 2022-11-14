// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StickBombTpsAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STICKYBOMB_API UStickBombTpsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
