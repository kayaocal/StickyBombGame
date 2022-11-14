// Copyright Epic Games, Inc. All Rights Reserved.


#include "StickyBombCharacter.h"
#include "Projectile/StickyBombProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "StickyBombHUD.h"
#include "StickyBombPlayerState.h"
#include "StickyBombPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AStickyBombCharacter

AStickyBombCharacter::AStickyBombCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	WeaponAttachSocketName = "hand_r";
	ProjectileSpawnOffset = 120.0f;
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// otherwise won't be visible in the multiplayer
	FP_Gun->SetOwnerNoSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(GetMesh1P(), WeaponAttachSocketName);
	FP_Gun->SetRelativeLocation(FVector{-9.0f, 9.0f, -3.0f});
	FP_Gun->SetRelativeRotation(FRotator{15.0f, 90.0f, 10.0f});
	
	const auto Mesh3P = GetMesh();
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOnlyOwnerSee(false);
	TP_Gun->SetOwnerNoSee(true);	
	TP_Gun->bCastDynamicShadow = false;
	TP_Gun->CastShadow = false;
	TP_Gun->SetupAttachment(Mesh3P, WeaponAttachSocketName);
	TP_Gun->SetRelativeLocation(FVector{-10.0f, 6.0f, -2.0f});
	TP_Gun->SetRelativeRotation(FRotator{0.0f, 90.0f, 0.0f});
	
}

void AStickyBombCharacter::InsertLootableAmmo(AStickyBombProjectile* Projectile)
{
	ensure(Projectile);
	LootableProjectile.Push(Projectile);
	if(LootableProjectile.Num() == 1)
	{
		OnLootableAmmoAroundExistChaged.Broadcast(true);
	}
}

void AStickyBombCharacter::RemoveLootableAmmo(AStickyBombProjectile* Projectile)
{
	ensure(Projectile);
	if(LootableProjectile.Contains(Projectile))
	{
		LootableProjectile.Remove(Projectile);
	}
	
	if(LootableProjectile.Num() == 0)
	{
		OnLootableAmmoAroundExistChaged.Broadcast(false);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AStickyBombCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AStickyBombCharacter::OnFire);
	PlayerInputComponent->BindAction("TakeAmmo", IE_Released, this, &AStickyBombCharacter::TakeAmmo);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AStickyBombCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStickyBombCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AStickyBombCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AStickyBombCharacter::LookUpAtRate);
}

bool AStickyBombCharacter::CanFire() const
{
	if(const auto PS = GetPlayerState<AStickyBombPlayerState>())
	{
		if(PS->GetAmmoCount() > 0)
		{
			return true;
		}
	}

	return false;
}

void AStickyBombCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	StickyPlayerController = GetController<AStickyBombPlayerController>();
}

void AStickyBombCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if(const auto PC = Cast<AStickyBombPlayerController>(NewController))
	{
		if(const auto Hud = PC->GetHUD<AStickyBombHUD>())
		{
			Hud->SetUpCharacter(this);
		}
	}
}

void AStickyBombCharacter::UnPossessed()
{
	if(const auto PC = Cast<AStickyBombPlayerController>(GetController()))
	{
		if(const auto Hud = PC->GetHUD<AStickyBombHUD>())
		{
			Hud->CleanUpCharacter(this);
		}
	}
	Super::UnPossessed();
}



void AStickyBombCharacter::Server_Fire_Implementation(FVector FireLocation, FVector FireDirection)
{
	if(CanFire())
	{
		if(const auto PS = GetPlayerState<AStickyBombPlayerState>())
		{
			PS->DecrementAmmoCount();

			FActorSpawnParameters ActorSpawnParams;
	 		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			// spawn the projectile at the muzzle
			const auto Projectile = GetWorld()->SpawnActor<AStickyBombProjectile>(ProjectileClass, FireLocation, FireDirection.Rotation(), ActorSpawnParams);
			Projectile->IgnoreActor(this);
			NetMulticast_OnFired();
		}
		
	}
}

void AStickyBombCharacter::NetMulticast_OnFired_Implementation()
{
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if(FireAnimationTPS != nullptr)
	{
		PlayAnimMontage(FireAnimationTPS);
	}
}

void AStickyBombCharacter::OnFire()
{
	if(!CanFire())
	{
		return;
	}

	const FVector ProjectileLocation = GetFirstPersonCameraComponent()->GetComponentLocation() +  GetFirstPersonCameraComponent()->GetForwardVector() * ProjectileSpawnOffset;
	Server_Fire(ProjectileLocation, GetFirstPersonCameraComponent()->GetForwardVector());
}

void AStickyBombCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AStickyBombCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AStickyBombCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AStickyBombCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AStickyBombCharacter::TakeAmmo()
{
	if(LootableProjectile.Num() > 0)
	{
		Server_TakeAmmo();
	}
}

void AStickyBombCharacter::Server_TakeAmmo_Implementation()
{
	if(LootableProjectile.Num() > 0)
	{
		const auto Loot = LootableProjectile[0];
		LootableProjectile.Remove(Loot);
		Loot->Destroy();
		if(const auto PS = GetPlayerState<AStickyBombPlayerState>())
		{
			PS->IncrementAmmoCount();
		}
	}
}