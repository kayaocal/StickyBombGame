// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StickyBombCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class AStickyBombPlayerController;
class AStickyBombProjectile;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLootableAmmoAroundExistChanged, bool, bNewLootableAmmoExist);



UCLASS(config=Game)
class AStickyBombCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName WeaponAttachSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ProjectileSpawnOffset;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AStickyBombProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimationTPS;


	UPROPERTY(BlueprintAssignable, Category = "StickyBombCharacterEvents")
	FLootableAmmoAroundExistChanged OnLootableAmmoAroundExistChaged;
	
	/** Returns Mesh1P sub-object **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent sub-object **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	AStickyBombCharacter();
	
	void InsertLootableAmmo(AStickyBombProjectile* Projectile);
	
	void RemoveLootableAmmo(AStickyBombProjectile* Projectile);
	
	virtual void OnRep_Controller() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void UnPossessed() override;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	bool CanFire() const;
	
	void TakeAmmo();
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

private:

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* TP_Gun;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UFUNCTION(Server, Reliable)
	void Server_Fire(FVector FireLocation, FVector FireDirection);

	UFUNCTION(Server, Reliable)
	void Server_TakeAmmo();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnFired();
	
	UPROPERTY()
	AStickyBombPlayerController* StickyPlayerController;

	UPROPERTY()
	TArray<AStickyBombProjectile*> LootableProjectile;
	
};

