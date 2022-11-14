// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "StickyBombCharacter.h"
#include "GameFramework/HUD.h"
#include "StickyBombHUD.generated.h"

UCLASS()
class AStickyBombHUD : public AHUD
{
	GENERATED_BODY()

public:
	AStickyBombHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	
	void SetUpPlayerState(class AStickyBombPlayerState* PlayerState);

	void CleanUpPlayerState(class AStickyBombPlayerState* PlayerState);

	void SetUpCharacter(AStickyBombCharacter* Character);

	void CleanUpCharacter(AStickyBombCharacter* Character);

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	uint8 AmmoCount;
	
	bool bLootableAmmoExist;
	
	UFUNCTION()
	void OnAmmoCountChanged(uint8 NewAmmoCount);

	UFUNCTION()
	void OnLootableAmmoExistChanged(bool bNewValue);

	
};

