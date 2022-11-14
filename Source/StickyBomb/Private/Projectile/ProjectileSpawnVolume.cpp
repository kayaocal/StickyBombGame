// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileSpawnVolume.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile/StickyBombProjectile.h"


// Sets default values
AProjectileSpawnVolume::AProjectileSpawnVolume()
{
	LineTraceLenght = 500.0f;
	ItemHalfSizeZ = 15.0f;
	RandomSuccessRate = 40;
	VolumeUpdateTime = 5;
}

// Called when the game starts or when spawned
void AProjectileSpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if(GetNetMode() == ENetMode::NM_Client)
	{
		Destroy();
	}

	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_VolumeTimer))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_VolumeTimer, this, &AProjectileSpawnVolume::VolumeTimer, VolumeUpdateTime, true);
	}

	const FBoxSphereBounds Bounds = GetBounds();
	Size = Bounds.GetBox().GetSize();
	Origin = Bounds.Origin;
}

void AProjectileSpawnVolume::VolumeTimer() const
{
	const int Rand = FMath::RandRange(0, 100);
	if(Rand < RandomSuccessRate)
	{
		FHitResult Result;
		GetWorld()->LineTraceSingleByChannel(Result, DecideSpawnLocation(), FVector::DownVector * LineTraceLenght, ECollisionChannel::ECC_WorldStatic);
		if(Result.bBlockingHit)
		{
			const auto Projectile = GetWorld()->SpawnActor<AStickyBombProjectile>(ProjectileClass, Result.ImpactPoint + FVector::UpVector * ItemHalfSizeZ, FRotator::ZeroRotator);
			Projectile->GetProjectileMovement()->StopMovementImmediately();
			Projectile->GetProjectileMovement()->SetActive(false);
		}
		
	}
}

/** Just Random Point in volume bounds */
FVector AProjectileSpawnVolume::DecideSpawnLocation() const
{
	FVector RandLocation = Origin;
	RandLocation.X += (FMath::RandRange(0.0f, Size.X)-(Size.X*0.5f));
	RandLocation.Y += (FMath::RandRange(0.0f, Size.Y)-(Size.Y*0.5f));
	RandLocation.Z += Size.Z * 0.5f;
	return RandLocation;
}