// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileSpawnVolume.generated.h"

UCLASS()
class STICKYBOMB_API AProjectileSpawnVolume : public AVolume
{
	GENERATED_BODY()

public:

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	TSubclassOf<class AStickyBombProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float LineTraceLenght;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ItemHalfSizeZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	int RandomSuccessRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float VolumeUpdateTime;
	
	// Sets default values for this actor's properties
	AProjectileSpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void VolumeTimer() const;

private:
	
	FVector DecideSpawnLocation() const;
	
	FTimerHandle TimerHandle_VolumeTimer;
	
	FVector Origin;
	
	FVector Size;

};
