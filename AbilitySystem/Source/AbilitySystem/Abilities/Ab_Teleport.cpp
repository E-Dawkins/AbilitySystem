// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_Teleport::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (!PlayerPtr)
	{
		return;
	}

	TeleportLocation = PlayerPtr->GetActorLocation();
	CursorLocation = TeleportLocation - FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	
	// Spawn the teleport cursors, so player knows where they are teleporting
	if (const auto World = PlayerPtr->GetWorld())
	{
		NormalCursorPtr = World->SpawnActor(NormalCursor);
		LedgeCursorPtr = World->SpawnActor(LedgeCursor);
		CrouchCursorPtr = World->SpawnActor(CrouchCursor);
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
	PlayerPtr->SetActorLocation(TeleportLocation);
}

void UAb_Teleport::Update(float _DeltaSeconds)
{
	if (!PlayerPtr)
	{
		return;
	}

	// -- Update cursors' location
	GetTeleportVariables();
	
	if (NormalCursorPtr)
	{
		NormalCursorPtr->SetActorLocation(CursorLocation);
		NormalCursorPtr->SetActorHiddenInGame(bCanMantle || bShouldCrouch);
	}

	if (LedgeCursorPtr)
	{
		LedgeCursorPtr->SetActorLocation(CursorLocation);
		LedgeCursorPtr->SetActorHiddenInGame(!bCanMantle || bShouldCrouch);
	}

	if (CrouchCursorPtr)
	{
		CrouchCursorPtr->SetActorLocation(CursorLocation);
		CrouchCursorPtr->SetActorHiddenInGame(bCanMantle || !bShouldCrouch);
	}

	if (bDrawStats)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, bCanMantle ? FColor::Green : FColor::Red, FString("Into Mantle"));
		GEngine->AddOnScreenDebugMessage(-1, -1, bShouldCrouch ? FColor::Green : FColor::Red, FString("Into Crouch"));
	}
}

void UAb_Teleport::OnDeactivation()
{
	// Destroy the cursors
	NormalCursorPtr->Destroy();
	LedgeCursorPtr->Destroy();
	CrouchCursorPtr->Destroy();
}

void UAb_Teleport::GetTeleportVariables()
{
	// -- Set defaults --
	{
		bCanMantle = false;
		bShouldCrouch = false;
	}

	const float PlayerHalfHeight = PlayerPtr->GetSimpleCollisionHalfHeight();
	const float PlayerCrouchedHalfHeight = PlayerPtr->GetCharacterMovement()->CrouchedHalfHeight;
	const float PlayerRadius = PlayerPtr->GetSimpleCollisionRadius();

	bool bLocationsSet = false;
	FVector FinalTpLocation = TeleportLocation;

	FHitResult InitialTraceHit;

	// -- Initial line trace to determine if we are looking at a blocking collider --
	{
		FVector TraceStart;
		FRotator TraceDirection;

		PlayerPtr->GetActorEyesViewPoint(TraceStart, TraceDirection);

		const bool bInitialTraceResult = PlayerPtr->GetWorld()->LineTraceSingleByChannel
		(
			InitialTraceHit,
			TraceStart,
			TraceStart + PlayerPtr->GetControlRotation().Vector() * TeleportRange,
			ECC_Visibility
		);

		if (!bInitialTraceResult) // no trace hit, tp location is mid-air
		{
			FinalTpLocation = InitialTraceHit.TraceEnd + -TraceDirection.Vector() * PlayerRadius + FVector::UpVector * PlayerHalfHeight;

			bLocationsSet = true;
		}
	}

	// -- Check for mantle on wall --
	if (!bLocationsSet)
	{
		const float Dot = FMath::Abs(FVector::DotProduct(FVector::UpVector, InitialTraceHit.ImpactNormal));
		const bool bMantleAble = Dot <= WallDotTolerance;

		TArray<FHitResult> HitResultArr;

		const FVector NormalUpVector = GetUpFromForward(InitialTraceHit.ImpactNormal);

		if (bMantleAble)
		{
			constexpr float SphereTraceRadius = 10.f;
			
			RecursiveSphereTrace
			(
				InitialTraceHit.ImpactPoint,
				InitialTraceHit.ImpactPoint,
				InitialTraceHit.ImpactPoint + NormalUpVector * EdgeTolerance * 2.f,
				SphereTraceRadius,
				HitResultArr
			);

			bCanMantle = FVector::Dist(InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;

			if (bDrawMantle)
			{
				DrawDebugLine(PlayerPtr->GetWorld(), InitialTraceHit.ImpactPoint, InitialTraceHit.ImpactPoint + NormalUpVector * 100.f, MantleDirection, false, -1.f, 0, 3.f);

				for (auto& Hit : HitResultArr)
				{
					DrawDebugSphere(PlayerPtr->GetWorld(), Hit.ImpactPoint, 10.f, 12, bCanMantle ? SphereTraceSucceed : SphereTraceFail);
				}
			}
		}

		constexpr float CmTolerance = 5.f;
		const FVector PlayerExtents = PlayerPtr->GetSimpleCollisionCylinderExtent();

		if (bCanMantle)
		{
			FinalTpLocation = HitResultArr.Last().ImpactPoint + FVector::UpVector * (PlayerHalfHeight + CmTolerance * 2.f);
		}
		else
		{
			FinalTpLocation = InitialTraceHit.ImpactPoint + InitialTraceHit.ImpactNormal * (PlayerExtents + FVector(CmTolerance));
		}
	}

	{
		bool bCanTeleport;
		FVector DepenetrationVector;
		FVector TpLocationNoOffset = FinalTpLocation;
		
		// Check if there is room to teleport player standing up
		bCanTeleport = FreeHeadRoom(FinalTpLocation, PlayerHalfHeight, DepenetrationVector); // standing check #1

		if (!bCanTeleport) // cant teleport standing in orig location, try with penetration offset
		{
			FinalTpLocation += DepenetrationVector;
			bCanTeleport = FreeHeadRoom(FinalTpLocation, PlayerHalfHeight, DepenetrationVector); // standing check #2

			if (!bCanTeleport && !PlayerPtr->bIsCrouched) // no standing room and not already crouched, check if the player can teleport crouching
			{
				FinalTpLocation = TpLocationNoOffset - FVector::UpVector * (PlayerHalfHeight - PlayerCrouchedHalfHeight);
				bShouldCrouch = FreeHeadRoom(FinalTpLocation, PlayerCrouchedHalfHeight, DepenetrationVector); // crouch check #1

				if (!bShouldCrouch) // cant teleport crouching in orig location, try with penetration offset
				{
					FinalTpLocation += DepenetrationVector;
					bShouldCrouch = FreeHeadRoom(FinalTpLocation, PlayerCrouchedHalfHeight, DepenetrationVector); // crouch check #2
				}

				bCanTeleport = bShouldCrouch;
			}
		}

		// Set cursor and teleport location, if the new location is valid
        if (bCanTeleport)
        {
        	TeleportLocation = FinalTpLocation;
        	CursorLocation = TeleportLocation - FVector::UpVector * PlayerHalfHeight;
        }
	}
}

void UAb_Teleport::RecursiveSphereTrace(const FVector _OrigStart, const FVector _Start, const FVector _End,
		const float _Radius, TArray<FHitResult>& _OutHits, int _MaxIterations)
{
	if (_MaxIterations == 0) // if we have exceeded max iterations, early return
	{
		return;
	}

	_MaxIterations -= 1;

	// Perform single sphere trace
	FHitResult Hit;
	const TArray<AActor*> IgnoredActors;

	UKismetSystemLibrary::SphereTraceSingle
	(
		PlayerPtr,
		_Start,
		_End,
		_Radius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false,
		IgnoredActors,
		EDrawDebugTrace::None,
		Hit,
		true
	);

	// if sphere trace succeeded, recurse the function
	if (Hit.bBlockingHit)
	{
		_OutHits.Add(Hit);

		if (Hit.ImpactPoint.Equals(_End)) // (this will happen very rarely)
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

		// Re-call this function, with offset starting vector
		RecursiveSphereTrace(_OrigStart, Hit.ImpactPoint + Direction * _Radius * 2.f, _End, _Radius, _OutHits, _MaxIterations);
	}
}

bool UAb_Teleport::FreeHeadRoom(FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck, FVector& _DepenetrationVector) const
{
	FHitResult Hit;

	const float PlayerRadius = PlayerPtr->GetSimpleCollisionRadius();

	FCollisionQueryParams QueryParams;
	QueryParams.bFindInitialOverlaps = true; // life-saver, makes the depenetration vector work when trace starts penetrated

	const bool ObjectBlocking = PlayerPtr->GetWorld()->SweepSingleByChannel
	(
		Hit,
		_PlayerCenterAtNewLocation,
		_PlayerCenterAtNewLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeCapsule(FVector(PlayerRadius, PlayerRadius, _PlayerHalfHeightToCheck)),
		QueryParams
	);

	if (bDrawHeadRoom)
	{
		DrawDebugCapsule
		(
			PlayerPtr->GetWorld(),
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
