// Copyright Epic Games, Inc. All Rights Reserved.

#include "StickyBombGameMode.h"
#include "StickyBombHUD.h"
#include "StickyBombCharacter.h"
#include "StickyBombPlayerController.h"
#include "StickyBombPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AStickyBombGameMode::AStickyBombGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerStateClass = AStickyBombPlayerState::StaticClass();
	PlayerControllerClass = AStickyBombPlayerController::StaticClass();
	// use our custom HUD class
	HUDClass = AStickyBombHUD::StaticClass();
}
