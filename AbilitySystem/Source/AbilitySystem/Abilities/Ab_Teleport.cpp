// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	if (!Player)
	{
		return;
	}

	// Check if player should be crouched
	if (bShouldCrouch)
	{
		Player->Crouch();
	}

	// Teleport the player
	if (bCanTeleport)
	{
		Player->SetActorLocation(TeleportLocation);
	}
}

void UAb_Teleport::Update(APlayerCharacter* Player, float DeltaSeconds)
{
	// -- Update cursors transform (rotation, scale, location) --
	GetTeleportVariables(Player);
	
	if (NormalCursorPtr)
	{
		NormalCursorPtr->SetActorRotation(NormalCursor.Rotation);
		NormalCursorPtr->SetActorScale3D(NormalCursor.Scale);

		NormalCursorPtr->SetActorLocation(CursorLocation);
		NormalCursorPtr->SetActorHiddenInGame(!bCanTeleport || bCanMantle);
	}

	if (LedgeCursorPtr)
	{
		LedgeCursorPtr->SetActorRotation(LedgeCursor.Rotation);
		LedgeCursorPtr->SetActorScale3D(LedgeCursor.Scale);

		LedgeCursorPtr->SetActorLocation(CursorLocation);
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
void UAb_Teleport::GetTeleportVariables(APlayerCharacter* _Player)
{
	// Player not valid, early return
	if (!IsValid(_Player))
	{
		return;
	}

	// -- Set defaults --
	{
		TeleportLocation = _Player->GetActorLocation();
		CursorLocation = TeleportLocation;
		bCanTeleport = false;
		bCanMantle = false;
		bShouldCrouch = false;
	}

	bool bLocationsSet = false;

	const float TPOffsetCm = 10.f;
	const float PlayerHalfHeight = _Player->GetSimpleCollisionHalfHeight() + TPOffsetCm;
	const float PlayerRadius = _Player->GetSimpleCollisionRadius() + TPOffsetCm;
	const float PlayerExtentDiag = FMath::Sqrt(FMath::Square(PlayerHalfHeight) + FMath::Square(PlayerRadius)) + TPOffsetCm;

	FHitResult InitialTraceHit;

	// -- Initial line trace to determine if we are looking at a blocking collider --
	{
		FVector TraceStart;
		FRotator TraceDirection;

		_Player->GetActorEyesViewPoint(TraceStart, TraceDirection);

		bool bInitialTraceResult = _Player->GetWorld()->LineTraceSingleByChannel
		(
			InitialTraceHit,
			TraceStart,
			TraceStart + _Player->GetControlRotation().Vector() * TeleportRange,
			ECollisionChannel::ECC_Visibility
		);

		if (!bInitialTraceResult) // no trace hit, tp location is mid-air
		{
			CursorLocation = InitialTraceHit.TraceEnd;
			TeleportLocation = CursorLocation + -TraceDirection.Vector() * PlayerRadius + FVector::UpVector * PlayerHalfHeight;

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
			CursorLocation = InitialTraceHit.ImpactPoint;
			TeleportLocation = CursorLocation + InitialTraceHit.Normal * PlayerExtentDiag;

			bLocationsSet = true;
		}

		if (AngleFromNegativeNormal <= 45.f)
		{
			CursorLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.Normal * PlayerExtentDiag;
			TeleportLocation = CursorLocation;

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
			_Player, 
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
		bCanMantle = FVector::Dist(InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;

		if (bDebug)
		{
			for (auto& Hit : HitResultArr)
			{
				DrawDebugSphere(_Player->GetWorld(), Hit.ImpactPoint, 10.f, 12, bCanMantle ? FColor::Green : FColor::Red);
			}
		}

		CursorLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.Normal;

		FVector PlayerViewFlattened = _Player->GetControlRotation().Vector().GetSafeNormal();
		PlayerViewFlattened.Z = 0.f;

		if (bCanMantle)
		{
			TeleportLocation = HitResultArr.Last().ImpactPoint + PlayerViewFlattened * PlayerRadius + FVector::UpVector * PlayerHalfHeight;
		}
		else
		{
			TeleportLocation = InitialTraceHit.ImpactPoint + -PlayerViewFlattened * PlayerRadius + FVector::UpVector * PlayerHalfHeight;
		}
	}

	// -- Check head room --
	{
		// Check if there is room to teleport player standing up
		bCanTeleport = FreeHeadRoom(_Player, TeleportLocation, _Player->GetSimpleCollisionHalfHeight());

		if (!bCanTeleport) // no standing room, check if the player can teleport crouching
		{
			float CrouchTPOffset = _Player->GetSimpleCollisionHalfHeight() - _Player->GetCharacterMovement()->CrouchedHalfHeight;
			TeleportLocation -= FVector::UpVector * CrouchTPOffset;

			bShouldCrouch = FreeHeadRoom(_Player, TeleportLocation, _Player->GetCharacterMovement()->CrouchedHalfHeight);
			bCanTeleport = bShouldCrouch;
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

bool UAb_Teleport::FreeHeadRoom(APlayerCharacter* _Player, FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck)
{
	FHitResult Hit;

	float PlayerRadius = _Player->GetSimpleCollisionRadius();

	bool ObjectBlocking = _Player->GetWorld()->SweepSingleByChannel
	(
		Hit,
		_PlayerCenterAtNewLocation,
		_PlayerCenterAtNewLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeCapsule(FVector(PlayerRadius, PlayerRadius, _PlayerHalfHeightToCheck))
	);

	if (bDebug)
	{
		DrawDebugCapsule
		(
			_Player->GetWorld(),
			_PlayerCenterAtNewLocation,
			_PlayerHalfHeightToCheck,
			PlayerRadius,
			FQuat::Identity,
			ObjectBlocking ? FColor::Red : FColor::Green
		);
	}

	return !ObjectBlocking;
}
