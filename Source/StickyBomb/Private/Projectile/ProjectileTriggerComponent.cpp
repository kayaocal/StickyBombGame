// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileTriggerComponent.h"

#include "StickyBombCharacter.h"
#include "Projectile/StickyBombProjectile.h"


UProjectileTriggerComponent::UProjectileTriggerComponent()
{
}

void UProjectileTriggerComponent::PostInitProperties()
{
	Super::PostInitProperties();
	SetBoxExtent(FVector{128.0f, 128.0f, 64.0f});
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionProfileName("BlockAll");
	SetGenerateOverlapEvents(true);
}

void UProjectileTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UProjectileTriggerComponent::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UProjectileTriggerComponent::OnOverlapEnd);
}

void UProjectileTriggerComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(const auto PlayerCharacter = Cast<AStickyBombCharacter>(OtherActor))
	{
		if(PlayerCharacter !=GetOwner()->GetAttachParentActor())
		{
			PlayerCharacter->InsertLootableAmmo(Cast<AStickyBombProjectile>(GetOwner()));		
		}
	}
}

void UProjectileTriggerComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if(const auto PlayerCharacter = Cast<AStickyBombCharacter>(OtherActor))
	{
		if(PlayerCharacter !=GetOwner()->GetAttachParentActor())
		{
			PlayerCharacter->RemoveLootableAmmo(Cast<AStickyBombProjectile>(GetOwner()));
		}
	}
}
