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
		FVector ViewLocation;
		FRotator ViewRotation;
		PlayerPtr->GetActorEyesViewPoint(ViewLocation, ViewRotation);

		const FVector NewLocation = ViewLocation + ViewRotation.Vector() * HeldOffset;
		SetActorLocation(NewLocation, true);

		// If held and outside player interaction range, just drop the object
		if (FVector::Dist(ViewLocation, GetActorLocation()) > PlayerPtr->GetInteractionRange())
		{
			Throw(false);
			bHeld = false;
		}
	}
}

void AI_Throwable::Pickup() const
{
	RootMesh->SetSimulatePhysics(false);
}

void AI_Throwable::Throw(bool _bWithForce) const
{
	RootMesh->SetSimulatePhysics(true);

	if (_bWithForce)
	{
		RootMesh->AddImpulse(PlayerPtr->GetViewRotation().Vector() * ThrowForce, NAME_None, false);
	}
}
