// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerController.h"
#include "StickyBombCharacter.h"
#include "StickyBombHUD.h"
#include "StickyBombPlayerState.h"


// Sets default values
AStickyBombPlayerController::AStickyBombPlayerController()
{
}

// Called when the game starts or when spawned
void AStickyBombPlayerController::BeginPlay()
{
	Super::BeginPlay();
	TryToSetupHUD();
}

void AStickyBombPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	TryToSetupHUD();
}

void AStickyBombPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	//Clean old player state from hud
	TryToCleanupHUD();
	Super::ClientSetHUD_Implementation(NewHUDClass);
	TryToSetupHUD();
}

void AStickyBombPlayerController::TryToSetupHUD() const
{
	if(const auto Hud = GetHUD<AStickyBombHUD>())
	{
		if(const auto PS = GetPlayerState<AStickyBombPlayerState>())
		{
			Hud->SetUpPlayerState(PS);
		}

		if(const auto Char = Cast<AStickyBombCharacter>(GetCharacter()))
		{
			Hud->SetUpCharacter(Char);
		}
	}

}

void AStickyBombPlayerController::TryToCleanupHUD() const
{
	if(const auto Hud = GetHUD<AStickyBombHUD>())
	{
		if(const auto PS = GetPlayerState<AStickyBombPlayerState>())
		{
			Hud->CleanUpPlayerState(PS);
		}
	}
}

