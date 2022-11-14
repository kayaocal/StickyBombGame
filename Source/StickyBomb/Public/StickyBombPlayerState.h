// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "StickyBombPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmmoCountChanged, uint8, AmmoCount);



UCLASS()
class STICKYBOMB_API AStickyBombPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category = "PlayerStateEvents")
	FAmmoCountChanged OnAmmoCountChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	int InitialAmmoCount;
	
	// Sets default values for this actor's properties
	AStickyBombPlayerState();

	void DecrementAmmoCount();
	
	void IncrementAmmoCount();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	uint8 GetAmmoCount() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(ReplicatedUsing=OnRep_AmmoCount)
	uint8 AmmoCount;

	UFUNCTION()
	virtual void OnRep_AmmoCount();

	void AmmoCountChanged() const;

	void SetAmmoCount(uint8 NetAmmoCount);
	
};
