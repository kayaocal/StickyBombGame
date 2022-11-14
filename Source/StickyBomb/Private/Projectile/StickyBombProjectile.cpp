// Copyright Epic Games, Inc. All Rights Reserved.

#include "Projectile/StickyBombProjectile.h"

#include "StickyBombCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AStickyBombProjectile::AStickyBombProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AStickyBombProjectile::OnHit);		// set up a notification for when this component hits something blocking
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	// Set as root component
	RootComponent = CollisionComp;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComp->SetupAttachment(GetCollisionComp());
	StaticMeshComp->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	SetReplicatingMovement(true);

	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMaxSpeed = 3000.0f;
	ProjectileInitialSpeed = 3000.0f;
	ProjectileLifeSpan = 8.0f;
	ProjectileLifeSpanSticked = 4.0f;
	ProjectileAlertCountDown = 3.0f;
	AlertBlinkColorExpireTimeSlow = 0.5f;
	AlertBlinkColorExpireTimeFast = 0.1f;

	InitialLifeSpan = ProjectileLifeSpan;
	CountDownTimer = ProjectileLifeSpan;
	BlinkTimer = 0.f;
	ColorChangeCounter = 0;
	AlertColorFirst = FColor::Red.ReinterpretAsLinear();
	AlertColorSecond = FColor::Blue.ReinterpretAsLinear();

	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = ProjectileInitialSpeed;
	ProjectileMovement->MaxSpeed = ProjectileMaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ProjectileTrigger = CreateDefaultSubobject<UProjectileTriggerComponent>(TEXT("ProjectileOverlapTrigger"));
	ProjectileTrigger->SetupAttachment(GetRootComponent());
}


void AStickyBombProjectile::SetLifeSpan(float InLifespan)
{
	Super::SetLifeSpan(InLifespan);
	CountDownTimer = InLifespan;
}

void AStickyBombProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CountDownTimer-=DeltaSeconds;
	if(CountDownTimer < ProjectileAlertCountDown)
	{
		BlinkTimer+=DeltaSeconds;

		const float BlinkExpireTime = FMath::Lerp(AlertBlinkColorExpireTimeFast, AlertBlinkColorExpireTimeSlow, (CountDownTimer / ProjectileAlertCountDown));

		if(BlinkTimer > BlinkExpireTime)
		{
			ColorChangeCounter++;
			BlinkTimer -= BlinkExpireTime;
			MeshDynamicMaterial->SetVectorParameterValue("DiffuseColor", ColorChangeCounter % 2 == 0 ? AlertColorSecond : AlertColorFirst);
		}
	}
}

void AStickyBombProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		// Only add impulse and destroy projectile if we hit a physics
		if((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
		{
			//After hit to another object, then let bomb to be able to stick player who fired
			CollisionComp->ClearMoveIgnoreActors();
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		}
		//Try to stick
		else if(const auto StickyBombChar = Cast<AStickyBombCharacter>(OtherActor))
		{
			//To prevent stable bombs to stick player
			if(GetProjectileMovement()->Velocity.SizeSquared() > 1.0f)
			{
				const auto ClosestBoneName = StickyBombChar->GetMesh()->FindClosestBone(Hit.ImpactPoint);
				AttachToComponent(StickyBombChar->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ClosestBoneName);
				StickyBombChar->RemoveLootableAmmo(this);	
				ProjectileMovement->StopMovementImmediately();
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				SetLifeSpan(ProjectileLifeSpanSticked);
			}
		}
	}
}

//To Prevent bomb to stick to same player who has been fired this bomb
void AStickyBombProjectile::IgnoreActor(AActor* IgnoreActor) const
{
	CollisionComp->IgnoreActorWhenMoving(IgnoreActor, true);
}

void AStickyBombProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(ProjectileLifeSpan);
	BlinkTimer = 0.0f;
	checkf(StaticMeshComp!=NULL, TEXT("StaticMeshComponent is null!"));
	
	//create dynamic material anywhere u like, Constructor or anywhere .
	MeshDynamicMaterial = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(0), this);
	StaticMeshComp->SetMaterial(0, MeshDynamicMaterial);
	checkf(MeshDynamicMaterial != NULL, TEXT("Cant create MeshDynamicMaterial, so it cant blink!"));
}
