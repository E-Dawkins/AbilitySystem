// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"

void UAb_Teleport::OnActivation(UWorld* World)
{
	// Spawn the teleport cursor, so player knows where they are teleporting
	if (World)
	{
		FTransform CursorTransform = FTransform(CursorRotation, FVector::ZeroVector, CursorScale);
		CursorPtr = World->SpawnActor(Cursor);
	}
}

void UAb_Teleport::OnUse(APlayerCharacter* Player)
{
	// Teleport the player to the cursors location
	if (Player && CursorPtr)
	{
		Player->SetActorLocation(CursorPtr->GetActorLocation());
	}
}

void UAb_Teleport::Update(APlayerCharacter* Player, float DeltaSeconds)
{
	// Update cursors transform (rotation, scale, location)
	if (CursorPtr)
	{
		CursorPtr->SetActorRotation(CursorRotation);
		CursorPtr->SetActorScale3D(CursorScale);
	}

	// Location by tracing
	FVector TeleportLocation = Player->GetActorLocation();
	bool CanTeleport = GetTeleportLocation(Player, TeleportLocation);

	CursorPtr->SetActorLocation(TeleportLocation);
	CursorPtr->SetActorHiddenInGame(!CanTeleport);
}

void UAb_Teleport::OnDeactivation()
{
	// Destroy the cursor, if it isn't being destroyed already
	if (IsValid(CursorPtr))
	{
		CursorPtr->Destroy();
	}
}

// Returns true if player is looking at a valid teleport location
bool UAb_Teleport::GetTeleportLocation(APlayerCharacter* Player, FVector& TeleportLocation)
{
	// Player not valid, early return
	if (!IsValid(Player))
	{
		return false;
	}

	// Line trace
	FHitResult Hit;

	FVector TraceStart = Player->GetActorLocation();
	FVector TraceEnd = TraceStart + Player->GetControlRotation().Vector() * TeleportRange;

	Player->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	TeleportLocation = Hit.ImpactPoint;

	return Hit.bBlockingHit;
}
