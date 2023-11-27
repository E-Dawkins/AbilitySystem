// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_Teleport::OnActivation(UWorld* _World)
{
	// Spawn the teleport cursors, so player knows where they are teleporting
	if (_World)
	{
		FTransform CursorTransform = FTransform(NormalCursor.Rotation, FVector::ZeroVector, NormalCursor.Scale);
		NormalCursorPtr = _World->SpawnActor(NormalCursor.CursorClass);

		CursorTransform = FTransform(LedgeCursor.Rotation, FVector::ZeroVector, LedgeCursor.Scale);
		LedgeCursorPtr = _World->SpawnActor(LedgeCursor.CursorClass);
	}
}

void UAb_Teleport::OnUse(APlayerCharacter* _Player)
{
	if (!_Player)
	{
		return;
	}

	// Check if player should be crouched
	if (bShouldCrouch)
	{
		_Player->Crouch();
	}

	// Teleport the player
	if (bCanTeleport)
	{
		_Player->SetActorLocation(TeleportLocation);
	}
}

void UAb_Teleport::Update(APlayerCharacter* _Player, float _DeltaSeconds)
{
	if (!_Player)
	{
		return;
	}

	// -- Update cursors transform (rotation, scale, location) --
	if (bUseNewSystem)
	{
		GetTeleportVariables2(_Player);
	}
	else
	{
		GetTeleportVariables(_Player);
	}
	
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
	if (!_Player)
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
	const float PlayerCrouchedHalfHeight = _Player->GetCharacterMovement()->CrouchedHalfHeight + TPOffsetCm;
	const float PlayerRadius = _Player->GetSimpleCollisionRadius() + TPOffsetCm;

	// from testing, these values performed the best
	const float AngleTolerance = 65.f;
	const float PlayerRadiusWithTolerance = PlayerRadius * 1.25f;

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
		if (AngleFromPositiveNormal <= AngleTolerance)
		{
			CursorLocation = InitialTraceHit.ImpactPoint;
			TeleportLocation = CursorLocation + InitialTraceHit.ImpactNormal * FVector(PlayerRadiusWithTolerance, PlayerRadiusWithTolerance, PlayerHalfHeight);

			bLocationsSet = true;
		}

		if (AngleFromNegativeNormal <= AngleTolerance)
		{
			CursorLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.ImpactNormal * FVector(PlayerRadiusWithTolerance, PlayerRadiusWithTolerance, PlayerHalfHeight);
			TeleportLocation = CursorLocation;

			bLocationsSet = true;
		}
	}

	// -- Recursive sphere trace to get distance to the top edge --
	if (!bLocationsSet)
	{
		TArray<FHitResult> HitResultArr;
		TArray<AActor*> IgnoredActors;

		RecursiveSphereTrace
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
			TeleportLocation = InitialTraceHit.ImpactPoint + -PlayerViewFlattened * PlayerRadiusWithTolerance + FVector::UpVector * PlayerHalfHeight;
		}
	}

	// -- Check head room --
	{
		// Check if there is room to teleport player standing up
		bCanTeleport = FreeHeadRoom(_Player, TeleportLocation, PlayerHalfHeight - TPOffsetCm);

		if (!bCanTeleport) // no standing room, check if the player can teleport crouching
		{
			float CrouchTPOffset = PlayerHalfHeight - PlayerCrouchedHalfHeight;
			TeleportLocation -= FVector::UpVector * CrouchTPOffset;

			bShouldCrouch = FreeHeadRoom(_Player, TeleportLocation, PlayerCrouchedHalfHeight - TPOffsetCm);
			bCanTeleport = bShouldCrouch;
		}
	}
}

void UAb_Teleport::GetTeleportVariables2(APlayerCharacter* _Player)
{
	// Player not valid, early return
	if (!_Player)
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

	const float CmTolerance = 10.f;
	const float PlayerHalfHeight = _Player->GetSimpleCollisionHalfHeight();
	const float PlayerCrouchedHalfHeight = _Player->GetCharacterMovement()->CrouchedHalfHeight;
	const float PlayerRadius = _Player->GetSimpleCollisionRadius();
	const float PlayerDiameter = PlayerRadius * 2.f;

	bool bLocationsSet = false;

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

	// -- Check for mantle --
	if (!bLocationsSet)
	{
		TArray<AActor*> IgnoredActors;
		TArray<FHitResult> HitResultArr;

		RecursiveSphereTrace
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
			true,
			HitResultArr
		);

		bCanMantle = FVector::Dist(InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;

		if (bDebug)
		{
			for (auto& Hit : HitResultArr)
			{
				DrawDebugSphere(_Player->GetWorld(), Hit.ImpactPoint, 10.f, 12, bCanMantle ? FColor::Green : FColor::Red);
			}
		}

		CursorLocation = InitialTraceHit.ImpactPoint;

		/*if (bCanMantle)
		{
			TeleportLocation = HitResultArr.Last().ImpactPoint + FVector::UpVector * (PlayerHalfHeight + CmTolerance);
		}
		else*/
		{
			TeleportLocation = CursorLocation + InitialTraceHit.ImpactNormal * FVector(PlayerDiameter, PlayerDiameter, PlayerHalfHeight + CmTolerance);
		}
	}

	// -- Check head room --
	{
		// Check if there is room to teleport player standing up
		bCanTeleport = FreeHeadRoom(_Player, TeleportLocation, PlayerHalfHeight);

		if (!bCanTeleport) // no standing room, check if the player can teleport crouching
		{
			float CrouchTPOffset = PlayerHalfHeight - PlayerCrouchedHalfHeight;
			TeleportLocation -= FVector::UpVector * CrouchTPOffset;

			bShouldCrouch = FreeHeadRoom(_Player, TeleportLocation, PlayerCrouchedHalfHeight);
			bCanTeleport = bShouldCrouch;
		}
	}
}

void UAb_Teleport::RecursiveSphereTrace(const UObject* _WorldContextObject, const FVector _OrigStart, const FVector _Start, const FVector _End, float _Radius, ECollisionChannel _TraceChannel, 
	bool _bTraceComplex, const TArray<AActor*>& _IgnoredActors, EDrawDebugTrace::Type _DrawDebugType, bool _bIgnoreSelf, TArray<FHitResult>& _OutHits, int _MaxIterations)
{
	if (_MaxIterations == 0) // if we have exceeded max iterations, early return
	{
		return;
	}

	_MaxIterations -= 1;

	// Perform single sphere trace
	FHitResult Hit;

	UKismetSystemLibrary::SphereTraceSingle
	(
		_WorldContextObject,
		_Start,
		_End,
		_Radius,
		UEngineTypes::ConvertToTraceType(_TraceChannel),
		_bTraceComplex,
		_IgnoredActors,
		_DrawDebugType,
		Hit,
		_bIgnoreSelf
	);

	// if sphere trace succeeded, recurse the function
	if (Hit.bBlockingHit)
	{
		_OutHits.Add(Hit); // ad the trace hit

		if (Hit.ImpactPoint.Equals(_End)) // if the hit is exactly equal to trace end, early return (this will happen very rarely)
		{
			return;
		}

		// Check if the original direction is equal to the current direction
		// So when the passed in trace start passes the end point, early return
		FVector OrigDirection = (_End - _OrigStart).GetSafeNormal();
		FVector Direction = (_End - _Start).GetSafeNormal();

		if (FVector::DotProduct(OrigDirection, Direction) < 0)
		{
			return;
		}

		RecursiveSphereTrace(_WorldContextObject, _OrigStart, Hit.ImpactPoint + Direction * _Radius * 2.f, _End, _Radius, _TraceChannel,
			_bTraceComplex, _IgnoredActors, _DrawDebugType, _bIgnoreSelf, _OutHits, _MaxIterations);
	}
}

bool UAb_Teleport::FreeHeadRoom(APlayerCharacter* _Player, FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck)
{
	if (!_Player)
	{
		return false;
	}

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
