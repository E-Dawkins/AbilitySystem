// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_Teleport::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);
	
	// Spawn the teleport cursors, so player knows where they are teleporting
	if (const auto World = _Player->GetWorld())
	{
		FTransform CursorTransform = FTransform(NormalCursor.Rotation, FVector::ZeroVector, NormalCursor.Scale);
		NormalCursorPtr = World->SpawnActor(NormalCursor.CursorClass, &CursorTransform);

		CursorTransform = FTransform(LedgeCursor.Rotation, FVector::ZeroVector, LedgeCursor.Scale);
		LedgeCursorPtr = World->SpawnActor(LedgeCursor.CursorClass);

		CursorTransform = FTransform(CrouchCursor.Rotation, FVector::ZeroVector, CrouchCursor.Scale);
		CrouchCursorPtr = World->SpawnActor(CrouchCursor.CursorClass);
	}
}

void UAb_Teleport::OnUse()
{
	if (!PlayerPtr)
	{
		return;
	}

	// Check if player should be crouched
	if (bShouldCrouch)
	{
		PlayerPtr->Crouch();
	}

	// Teleport the player
	if (bCanTeleport)
	{
		PlayerPtr->SetActorLocation(TeleportLocation);
	}
}

void UAb_Teleport::Update(float _DeltaSeconds)
{
	if (!PlayerPtr)
	{
		return;
	}

	// -- Update cursors transform (rotation, scale, location) --
	GetTeleportVariables(PlayerPtr);
	
	if (NormalCursorPtr)
	{
		NormalCursorPtr->SetActorRotation(NormalCursor.Rotation);
		NormalCursorPtr->SetActorScale3D(NormalCursor.Scale);

		NormalCursorPtr->SetActorLocation(CursorLocation);
		NormalCursorPtr->SetActorHiddenInGame(!bCanTeleport || bCanMantle || bShouldCrouch);
	}

	if (LedgeCursorPtr)
	{
		LedgeCursorPtr->SetActorRotation(LedgeCursor.Rotation);
		LedgeCursorPtr->SetActorScale3D(LedgeCursor.Scale);

		LedgeCursorPtr->SetActorLocation(CursorLocation);
		LedgeCursorPtr->SetActorHiddenInGame(!bCanTeleport || !bCanMantle || bShouldCrouch);
	}

	if (CrouchCursorPtr)
	{
		CrouchCursorPtr->SetActorRotation(CrouchCursor.Rotation);
		CrouchCursorPtr->SetActorScale3D(CrouchCursor.Scale);

		CrouchCursorPtr->SetActorLocation(CursorLocation);
		CrouchCursorPtr->SetActorHiddenInGame(!bCanTeleport || bCanMantle || !bShouldCrouch);
	}

	if (bDrawStats)
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

	if (IsValid(CrouchCursorPtr))
	{
		CrouchCursorPtr->Destroy();
	}
}

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
		CursorLocation = TeleportLocation - FVector::UpVector * _Player->GetSimpleCollisionHalfHeight();
		bCanTeleport = false;
		bCanMantle = false;
		bShouldCrouch = false;
	}

	const float PlayerHalfHeight = _Player->GetSimpleCollisionHalfHeight();
	const float PlayerCrouchedHalfHeight = _Player->GetCharacterMovement()->CrouchedHalfHeight;
	const float PlayerRadius = _Player->GetSimpleCollisionRadius();

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
			TeleportLocation = InitialTraceHit.TraceEnd + -TraceDirection.Vector() * PlayerRadius + FVector::UpVector * PlayerHalfHeight;

			bLocationsSet = true;
		}
	}

	// -- Check for mantle on wall --
	if (!bLocationsSet)
	{
		float Dot = FMath::Abs(FVector::DotProduct(FVector::UpVector, InitialTraceHit.ImpactNormal));
		bool bIsWall = Dot <= WallDotTolerance;

		TArray<FHitResult> HitResultArr;

		FVector NormalUpVector = GetUpFromForward(InitialTraceHit.ImpactNormal);

		if (bIsWall)
		{
			float SphereTraceRadius = 10.f;
			TArray<AActor*> IgnoredActors;
			
			RecursiveSphereTrace
			(
				_Player,
				InitialTraceHit.ImpactPoint,
				InitialTraceHit.ImpactPoint,
				InitialTraceHit.ImpactPoint + NormalUpVector * EdgeTolerance * 2.f,
				SphereTraceRadius,
				ECollisionChannel::ECC_Visibility,
				false,
				IgnoredActors,
				EDrawDebugTrace::None,
				true,
				HitResultArr
			);

			bCanMantle = FVector::Dist(InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;

			if (bDrawMantle)
			{
				DrawDebugLine(_Player->GetWorld(), InitialTraceHit.ImpactPoint, InitialTraceHit.ImpactPoint + NormalUpVector * 100.f, MantleDirection, false, -1.f, 0, 3.f);

				for (auto& Hit : HitResultArr)
				{
					DrawDebugSphere(_Player->GetWorld(), Hit.ImpactPoint, 10.f, 12, bCanMantle ? SphereTraceSucceed : SphereTraceFail);
				}
			}
		}

		constexpr float CmTolerance = 5.f;
		const FVector PlayerExtents = _Player->GetSimpleCollisionCylinderExtent();

		if (bCanMantle)
		{
			TeleportLocation = HitResultArr.Last().ImpactPoint + FVector::UpVector * (PlayerHalfHeight + CmTolerance * 2.f);
		}
		else
		{
			TeleportLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.ImpactNormal * (PlayerExtents + FVector(CmTolerance));
		}
	}

	FVector FinalTpLocation = TeleportLocation;

	{
		FVector DepenetrationVector;
		float StandCrouchHeightDiff = PlayerHalfHeight - PlayerCrouchedHalfHeight;

		// Check if there is room to teleport player standing up
		bCanTeleport = FreeHeadRoom(_Player, TeleportLocation, PlayerHalfHeight, DepenetrationVector); // standing check #1

		if (!bCanTeleport) // cant teleport standing in orig location, try with penetration offset
		{
			FinalTpLocation += DepenetrationVector;
			bCanTeleport = FreeHeadRoom(_Player, FinalTpLocation, PlayerHalfHeight, DepenetrationVector); // standing check #2

			if (!bCanTeleport && !_Player->bIsCrouched) // no standing room and not already crouched, check if the player can teleport crouching
			{
				FinalTpLocation = TeleportLocation - FVector::UpVector * StandCrouchHeightDiff;
				bShouldCrouch = FreeHeadRoom(_Player, FinalTpLocation, PlayerCrouchedHalfHeight, DepenetrationVector); // crouch check #1

				if (!bShouldCrouch) // cant teleport crouching in orig location, try with penetration offset
				{
					FinalTpLocation += DepenetrationVector;
					bShouldCrouch = FreeHeadRoom(_Player, FinalTpLocation, PlayerCrouchedHalfHeight, DepenetrationVector); // crouch check #2
				}

				bCanTeleport = bShouldCrouch;
			}
		}
	}

	// Set cursor and teleport location
	TeleportLocation = FinalTpLocation;
	CursorLocation = TeleportLocation - FVector::UpVector * PlayerHalfHeight;
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

		// Check if the original direction is facing same as the current direction
		// So when the passed in trace start passes the end point, early return
		const FVector OrigDirection = (_End - _OrigStart).GetSafeNormal();
		const FVector Direction = (_End - _Start).GetSafeNormal();

		if (FVector::DotProduct(OrigDirection, Direction) < 0)
		{
			return;
		}

		RecursiveSphereTrace(_WorldContextObject, _OrigStart, Hit.ImpactPoint + Direction * _Radius * 2.f, _End, _Radius, _TraceChannel,
			_bTraceComplex, _IgnoredActors, _DrawDebugType, _bIgnoreSelf, _OutHits, _MaxIterations);
	}
}

bool UAb_Teleport::FreeHeadRoom(APlayerCharacter* _Player, FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck, FVector& _DepenetrationVector) const
{
	if (!_Player)
	{
		return false;
	}

	FHitResult Hit;

	float PlayerRadius = _Player->GetSimpleCollisionRadius();

	FCollisionQueryParams QueryParams;
	QueryParams.bFindInitialOverlaps = true; // life-saver

	bool ObjectBlocking = _Player->GetWorld()->SweepSingleByChannel
	(
		Hit,
		_PlayerCenterAtNewLocation,
		_PlayerCenterAtNewLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeCapsule(FVector(PlayerRadius, PlayerRadius, _PlayerHalfHeightToCheck)),
		QueryParams
	);

	if (bDrawHeadRoom)
	{
		DrawDebugCapsule
		(
			_Player->GetWorld(),
			_PlayerCenterAtNewLocation,
			_PlayerHalfHeightToCheck,
			PlayerRadius,
			FQuat::Identity,
			ObjectBlocking ? HeadRoomFail : HeadRoomSucceed
		);
	}

	_DepenetrationVector = Hit.Normal * (Hit.PenetrationDepth + 2.5f); // 2.5cm lee-way around player

	return !ObjectBlocking;
}

FVector UAb_Teleport::GetUpFromForward(FVector _Forward)
{
	const FVector Right = FVector::CrossProduct(_Forward, FVector::UpVector).GetSafeNormal();
	return FVector::CrossProduct(Right, _Forward);
}
