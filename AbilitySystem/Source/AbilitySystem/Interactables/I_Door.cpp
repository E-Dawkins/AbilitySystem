// Fill out your copyright notice in the Description page of Project Settings.


#include "I_Door.h"

#include "AbilitySystem/_Misc/Helpers.h"

void AI_Door::BeginPlay()
{
	Super::BeginPlay();

	FRotator TempStartRotation = StaticMesh->GetComponentRotation();
	StartRotation = TempStartRotation;
	EndRotation = TempStartRotation.Add(OpenAnglePerAxis.X, OpenAnglePerAxis.Y, OpenAnglePerAxis.Z); // this changes original variable
}

bool AI_Door::StartInteract()
{
	if (!Super::StartInteract())
	{
		return false;
	}

	if (bDoorOpen && !bToggleable)
	{
		return false;
	}

	if (bDoorAnimating)
	{
		return false;
	}
	
	bDoorOpen = !bDoorOpen;
	TargetRotation = bDoorOpen ? EndRotation : StartRotation;

	AnimateDoor();
	
	return true;
}

void AI_Door::AnimateDoor()
{
	bDoorAnimating = true;
	
	const FRotator Current = FHelpers::MoveTowards(StaticMesh->GetComponentRotation(), TargetRotation, DoorOpenSpeed);
	StaticMesh->SetWorldRotation(Current);
	
	if (!Current.Equals(TargetRotation, DoorOpenSpeed))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AI_Door::AnimateDoor);

		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString("Door animating"));
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Purple, FString::Printf(TEXT("Current: %s"), *Current.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Purple, FString::Printf(TEXT("Target: %s"), *TargetRotation.ToString()));
	}
	else
	{
		bDoorAnimating = false;
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("Door not animating"));
	}
}
