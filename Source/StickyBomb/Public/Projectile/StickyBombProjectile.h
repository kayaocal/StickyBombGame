// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileTriggerComponent.h"
#include "StickyBombCharacter.h"
#include "GameFramework/Actor.h"
#include "StickyBombProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AStickyBombProjectile : public AActor
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ProjectileMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ProjectileInitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ProjectileLifeSpan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ProjectileLifeSpanSticked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float ProjectileAlertCountDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float AlertBlinkColorExpireTimeSlow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float AlertBlinkColorExpireTimeFast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	FLinearColor AlertColorFirst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	FLinearColor AlertColorSecond;

	AStickyBombProjectile();
	
	virtual void SetLifeSpan(float InLifespan) override;
	
	virtual void Tick(float DeltaSeconds) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp sub-object **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement sub-object **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
	void IgnoreActor(AActor* IgnoreActor) const;

protected:
	
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UStaticMeshComponent* StaticMeshComp;
	
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UProjectileTriggerComponent* ProjectileTrigger;
	
	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY()
	UMaterialInstanceDynamic* MeshDynamicMaterial;

	float CountDownTimer;
	float BlinkTimer;
	int ColorChangeCounter;
	
};


