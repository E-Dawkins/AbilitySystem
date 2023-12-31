// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractable.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Component"));
	SetRootComponent(RootMesh);
}

bool ABaseInteractable::OnHover()
{
	if (!bCanInteract)
	{
		return false;
	}
	
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("On Hover"));
	bHovered = true;

	return true;
}

bool ABaseInteractable::OnUnHover()
{
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("On Un-Hover"));
	bHovered = false;

	if (bInteractedWith)
	{
		EndInteract();
	}

	return true;
}

bool ABaseInteractable::StartInteract(APlayerCharacter* _PlayerPtr)
{
	if (!bCanInteract || !IsValid(_PlayerPtr))
	{
		return false;
	}

	PlayerPtr = _PlayerPtr;
	
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Purple, FString("Started Interaction"));
	bInteractedWith = true;

	return true;
}

bool ABaseInteractable::EndInteract()
{
	if (!bInteractedWith)
	{
		return false;
	}
	
	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Purple, FString("Ended Interaction"));
	bInteractedWith = false;

	return true;
}
