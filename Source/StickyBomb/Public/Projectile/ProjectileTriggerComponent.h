// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ProjectileTriggerComponent.generated.h"

/**
 * 
 */
UCLASS()
class STICKYBOMB_API UProjectileTriggerComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UProjectileTriggerComponent();
	
	virtual void PostInitProperties() override;

	virtual void BeginPlay() override;
	
protected:
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int OtherBodyIndex);

};
