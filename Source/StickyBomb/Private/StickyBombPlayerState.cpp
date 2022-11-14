// Fill out your copyright notice in the Description page of Project Settings.


#include "StickyBombPlayerState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AStickyBombPlayerState::AStickyBombPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	InitialAmmoCount = 3;
}

// Called when the game starts or when spawned
void AStickyBombPlayerState::BeginPlay()
{
	Super::BeginPlay();
	SetAmmoCount(InitialAmmoCount);
}

void AStickyBombPlayerState::DecrementAmmoCount()
{
	SetAmmoCount(AmmoCount-1);
}

void AStickyBombPlayerState::IncrementAmmoCount()
{
	SetAmmoCount(AmmoCount+1);
}

void AStickyBombPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStickyBombPlayerState, AmmoCount);

}

uint8 AStickyBombPlayerState::GetAmmoCount() const
{
	return AmmoCount;
}

void AStickyBombPlayerState::SetAmmoCount(const uint8 NewAmmoCount)
{
	if(HasAuthority() && AmmoCount != NewAmmoCount)
	{
		AmmoCount = NewAmmoCount;
		AmmoCountChanged();
	}
}

void AStickyBombPlayerState::OnRep_AmmoCount()
{
	AmmoCountChanged();
}

void AStickyBombPlayerState::AmmoCountChanged() const
{
	OnAmmoCountChanged.Broadcast(AmmoCount);
}



