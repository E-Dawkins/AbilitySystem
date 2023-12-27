// Fill out your copyright notice in the Description page of Project Settings.


#include "I_Door.h"

AI_Door::AI_Door()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AI_Door::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetActorRotation();
	DoorTimeline.SetTimelineLengthMode(TL_TimelineLength);
}

void AI_Door::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DoorTimeline.GetTimelineLength() != DoorOpenTime)
	{
		DoorTimeline.SetTimelineLength(DoorOpenTime);
	}

	const FRotator NewEndRotation = StartRotation + AnglePerAxis;
	if (EndRotation != NewEndRotation)
	{
		EndRotation = NewEndRotation;
	}
	
	DoorTimeline.TickTimeline(DeltaSeconds);

	const float LerpAlpha = DoorTimeline.GetPlaybackPosition() / DoorTimeline.GetTimelineLength();
	const FRotator NewDoorRotation = FMath::Lerp(StartRotation, EndRotation, LerpAlpha);

	if (!GetActorRotation().Equals(NewDoorRotation))
	{
		SetActorRotation(NewDoorRotation);
	}
}

bool AI_Door::StartInteract(APlayerCharacter* _PlayerPtr)
{
	if (!Super::StartInteract(_PlayerPtr) || (bDoorOpen && !bToggleable))
	{
		return false;
	}
	
	bDoorOpen = !bDoorOpen;

	if (bDoorOpen)
	{
		DoorTimeline.Play();
	}
	else
	{
		DoorTimeline.Reverse();
	}
	
	return true;
}
