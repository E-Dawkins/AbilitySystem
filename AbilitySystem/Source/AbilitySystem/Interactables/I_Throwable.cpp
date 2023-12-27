// Fill out your copyright notice in the Description page of Project Settings.


#include "I_Throwable.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

bool AI_Throwable::StartInteract(APlayerCharacter* _PlayerPtr)
{
	if (!Super::StartInteract(_PlayerPtr))
	{
		return false;
	}

	bHeld = !bHeld;

	if (bHeld)
	{
		Pickup();
	}
	else
	{
		Throw();
	}
	
	return true;
}

void AI_Throwable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHeld)
	{
		FVector RightOffset = PlayerPtr->GetActorForwardVector() * HeldOffset.X;
		FVector ForwardOffset = PlayerPtr->GetActorForwardVector() * HeldOffset.Y;
		FVector UpOffset = PlayerPtr->GetActorUpVector() * HeldOffset.Y;
		FVector NewLocation = PlayerPtr->GetActorLocation() + RightOffset + ForwardOffset + UpOffset;
		bool bSuccess = TeleportTo(NewLocation, GetActorRotation());
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("New Location: %s"), *NewLocation.ToString()));

		if (bSuccess)
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString("Success"));
		}
	}
}

void AI_Throwable::Pickup()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Pickup"));
}

void AI_Throwable::Throw()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("Throw"));
}
