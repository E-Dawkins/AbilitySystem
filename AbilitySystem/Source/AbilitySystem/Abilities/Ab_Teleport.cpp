// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"

void UAb_Teleport::OnActivation(UWorld* World)
{
	// Spawn the teleport cursors, so player knows where they are teleporting
	if (World)
	{
		FTransform CursorTransform = FTransform(NormalCursor.Rotation, FVector::ZeroVector, NormalCursor.Scale);
		NormalCursorPtr = World->SpawnActor(NormalCursor.CursorClass);

		CursorTransform = FTransform(LedgeCursor.Rotation, FVector::ZeroVector, LedgeCursor.Scale);
		LedgeCursorPtr = World->SpawnActor(LedgeCursor.CursorClass);
	}
}

void UAb_Teleport::OnUse(APlayerCharacter* Player)
{
	// Teleport the player
	if (Player && bCanTeleport)
	{
		Player->SetActorLocation(LocationToTeleport);
	}
}

void UAb_Teleport::Update(APlayerCharacter* Player, float DeltaSeconds)
{
	// -- Update cursors transform (rotation, scale, location) --
	GetTeleportLocation(Player, LocationToTeleport, LocationForCursor, bCanTeleport, bCanMantle);
	
	if (NormalCursorPtr)
	{
		NormalCursorPtr->SetActorRotation(NormalCursor.Rotation);
		NormalCursorPtr->SetActorScale3D(NormalCursor.Scale);

		NormalCursorPtr->SetActorLocation(LocationForCursor);
		NormalCursorPtr->SetActorHiddenInGame(!bCanTeleport || bCanMantle);
	}

	if (LedgeCursorPtr)
	{
		LedgeCursorPtr->SetActorRotation(LedgeCursor.Rotation);
		LedgeCursorPtr->SetActorScale3D(LedgeCursor.Scale);

		LedgeCursorPtr->SetActorLocation(LocationForCursor);
		LedgeCursorPtr->SetActorHiddenInGame(!bCanTeleport || !bCanMantle);
	}

	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, bCanTeleport ? FColor::Green : FColor::Red, FString("Teleport"));
		GEngine->AddOnScreenDebugMessage(-1, -1, bCanMantle ? FColor::Green : FColor::Red, FString("Mantle"));
	}
}

void UAb_Teleport::OnDeactivation()
{
	// Destroy the cursors, if they haven't been destroyed already
	if (IsValid(NormalCursorPtr))
	{
		NormalCursorPtr->Destroy();
	}

	if (IsValid(LedgeCursorPtr))
	{
		LedgeCursorPtr->Destroy();
	}
}

// Returns true if player is looking at a valid teleport location
void UAb_Teleport::GetTeleportLocation(APlayerCharacter* Player, FVector& TeleportLocation, FVector& CursorLocation, bool& CanTeleport, bool& CanMantle)
{
	// Player not valid, early return
	if (!IsValid(Player))
	{
		return;
	}

	FVector Local_TeleportLocation;
	FVector Local_CursorLocation;
	bool Local_bCanMantle;

	bool bLocationsSet = false;

	const float TPOffsetCm = 10.f;
	const float PlayerHalfHeight = Player->GetSimpleCollisionHalfHeight() + TPOffsetCm;
	const float PlayerRadius = Player->GetSimpleCollisionRadius() + TPOffsetCm;
	const float PlayerExtentDiag = FMath::Sqrt(FMath::Square(PlayerHalfHeight) + FMath::Square(PlayerRadius)) + TPOffsetCm;

	FHitResult InitialTraceHit;

	// -- Initial line trace to determine if we are looking at a blocking collider --
	{
		FVector TraceStart;
		FRotator TraceDirection;

		Player->GetActorEyesViewPoint(TraceStart, TraceDirection);

		bool bInitialTraceResult = Player->GetWorld()->LineTraceSingleByChannel
		(
			InitialTraceHit,
			TraceStart,
			TraceStart + Player->GetControlRotation().Vector() * TeleportRange,
			ECollisionChannel::ECC_Visibility
		);

		if (!bInitialTraceResult) // no trace hit, tp location is mid-air
		{
			Local_CursorLocation = InitialTraceHit.TraceEnd;
			Local_TeleportLocation = Local_CursorLocation + -TraceDirection.Vector() * PlayerRadius + FVector::UpVector * PlayerHalfHeight;

			bLocationsSet = true;
		}
	}

	// -- Check if we are aiming at the top or bottom of an object --
	if (!bLocationsSet)
	{
		float AngleFromPositiveNormal = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(InitialTraceHit.ImpactNormal, FVector(0, 0, 1))));
		float AngleFromNegativeNormal = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(InitialTraceHit.ImpactNormal, FVector(0, 0, -1))));

		// angle within 45 of postitive or negative normal, tp location offset by extent diag
		if (AngleFromPositiveNormal <= 45.f)
		{
			Local_CursorLocation = InitialTraceHit.ImpactPoint;
			Local_TeleportLocation = Local_CursorLocation + InitialTraceHit.Normal * PlayerExtentDiag;

			bLocationsSet = true;
		}

		if (AngleFromNegativeNormal <= 45.f)
		{
			Local_CursorLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.Normal * PlayerExtentDiag;
			Local_TeleportLocation = Local_CursorLocation;

			bLocationsSet = true;
		}
	}

	// -- Recursive sphere trace to get distance to the top edge --
	if (!bLocationsSet)
	{
		TArray<FHitResult> HitResultArr;
		TArray<AActor*> IgnoredActors;

		GetAllSphereTraceHits
		(
			Player, 
			InitialTraceHit.ImpactPoint, 
			InitialTraceHit.ImpactPoint, 
			InitialTraceHit.ImpactPoint + FVector::UpVector * EdgeTolerance * 2.f,
			10.f, 
			ECollisionChannel::ECC_Visibility, 
			false, 
			IgnoredActors, 
			EDrawDebugTrace::None, 
			false, 
			HitResultArr
		);

		// Check if we are aiming within edge tolerance threshold
		Local_bCanMantle = FVector::Dist(InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;

		if (bDebug)
		{
			for (auto& Hit : HitResultArr)
			{
				DrawDebugSphere(Player->GetWorld(), Hit.ImpactPoint, 10.f, 12, Local_bCanMantle ? FColor::Green : FColor::Red);
			}
		}

		Local_CursorLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.Normal;

		FVector PlayerViewFlattened = Player->GetControlRotation().Vector().GetSafeNormal();
		PlayerViewFlattened.Z = 0.f;

		if (Local_bCanMantle)
		{
			Local_TeleportLocation = HitResultArr.Last().ImpactPoint + PlayerViewFlattened * PlayerRadius + FVector::UpVector * PlayerHalfHeight;
		}
		else
		{
			Local_TeleportLocation = InitialTraceHit.ImpactPoint + -PlayerViewFlattened * PlayerRadius + FVector::UpVector * PlayerHalfHeight;
		}
	}

	// -- Set out parameters --
	{
		CanTeleport = FreeHeadRoom(Player, Local_TeleportLocation);
		CanMantle = Local_bCanMantle;

		if (CanTeleport)
		{
			TeleportLocation = Local_TeleportLocation;
			CursorLocation = Local_CursorLocation;
		}
	}
}

void UAb_Teleport::GetAllSphereTraceHits(const UObject* WorldContextObject, const FVector OrigStart, const FVector Start, const FVector End, float Radius, ECollisionChannel TraceChannel, 
	bool bTraceComplex, const TArray<AActor*>& IgnoredActors, EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, TArray<FHitResult>& OutHits, int MaxIterations)
{
	if (MaxIterations == 0)
	{
		return;
	}

	MaxIterations -= 1;

	FHitResult Hit;

	UKismetSystemLibrary::SphereTraceSingle
	(
		WorldContextObject,
		Start,
		End,
		Radius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		bTraceComplex,
		IgnoredActors,
		DrawDebugType,
		Hit,
		bIgnoreSelf
	);

	if (Hit.bBlockingHit)
	{
		OutHits.Add(Hit);

		if (Hit.ImpactPoint == End)
		{
			return;
		}

		FVector OrigDirection = (End - OrigStart).GetSafeNormal();
		FVector Direction = (End - Start).GetSafeNormal();

		if (OrigDirection.Equals(Direction) == false)
		{
			return;
		}

		GetAllSphereTraceHits(WorldContextObject, OrigStart, Hit.ImpactPoint + Direction * Radius * 2.f, End, Radius, TraceChannel,
			bTraceComplex, IgnoredActors, DrawDebugType, bIgnoreSelf, OutHits, MaxIterations);
	}
}

bool UAb_Teleport::FreeHeadRoom(APlayerCharacter* Player, FVector PlayerCenterAtNewLocation)
{
	FHitResult Hit;

	bool ObjectBlocking = Player->GetWorld()->SweepSingleByChannel
	(
		Hit,
		PlayerCenterAtNewLocation,
		PlayerCenterAtNewLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeCapsule(Player->GetSimpleCollisionCylinderExtent())
	);

	return !ObjectBlocking;
}
