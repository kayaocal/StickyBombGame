// Copyright Epic Games, Inc. All Rights Reserved.

#include "StickyBombHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "StickyBombPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AStickyBombHUD::AStickyBombHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}



void AStickyBombHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	Canvas->DrawColor = FColor::Red;

	
	Canvas->DrawText(GetStatsFont(), FString::Printf(TEXT("Ammo: %d"), AmmoCount), Canvas->ClipX * 0.9f, Canvas->ClipY * 0.9f);
	if(bLootableAmmoExist)
	{

		int32 TextSizeX, TextSizeY;
		//to center DrawText
		Canvas->WrappedPrint(false, Center.X , Canvas->ClipY * 0.8f, TextSizeX, TextSizeY, GetStatsFont(), 1.0f, 1.0f, false, false, TEXT("Press E to Pickup"), FFontRenderInfo());
		Canvas->DrawText(GetStatsFont(), TEXT("Press E to Pickup"), Center.X - TextSizeX * 0.5f, Canvas->ClipY * 0.8f - TextSizeY * 0.5f);
		
	}
	
}


void AStickyBombHUD::SetUpPlayerState(AStickyBombPlayerState* PlayerState)
{
	checkf(PlayerState!=NULL, TEXT("PlayerState is null"));
	if(!PlayerState->OnAmmoCountChanged.IsAlreadyBound(this, &AStickyBombHUD::OnAmmoCountChanged))
	{
		PlayerState->OnAmmoCountChanged.AddDynamic(this, &AStickyBombHUD::OnAmmoCountChanged);
	}
	AmmoCount = PlayerState->GetAmmoCount();
}


void AStickyBombHUD::CleanUpPlayerState(AStickyBombPlayerState* PlayerState)
{
	checkf(PlayerState!=NULL, TEXT("PlayerState is null"));
	if(PlayerState->OnAmmoCountChanged.IsAlreadyBound(this, &AStickyBombHUD::OnAmmoCountChanged))
	{
		PlayerState->OnAmmoCountChanged.RemoveDynamic(this, &AStickyBombHUD::OnAmmoCountChanged);
	}
}

void AStickyBombHUD::SetUpCharacter(AStickyBombCharacter* Character)
{
	checkf(Character!=NULL, TEXT("Character is null"));
	if(!Character->OnLootableAmmoAroundExistChaged.IsAlreadyBound(this, &AStickyBombHUD::OnLootableAmmoExistChanged))
	{
		Character->OnLootableAmmoAroundExistChaged.AddDynamic(this, &AStickyBombHUD::OnLootableAmmoExistChanged);
	}
	bLootableAmmoExist = false;
}

void AStickyBombHUD::CleanUpCharacter(AStickyBombCharacter* Character)
{
	if(Character->OnLootableAmmoAroundExistChaged.IsAlreadyBound(this, &AStickyBombHUD::OnLootableAmmoExistChanged))
	{
		Character->OnLootableAmmoAroundExistChaged.RemoveDynamic(this, &AStickyBombHUD::OnLootableAmmoExistChanged);
	}
}

void AStickyBombHUD::OnAmmoCountChanged(uint8 NewAmmoCount)
{
	AmmoCount = NewAmmoCount;
	
}

void AStickyBombHUD::OnLootableAmmoExistChanged(bool bNewValue)
{
	bLootableAmmoExist = bNewValue;
}
