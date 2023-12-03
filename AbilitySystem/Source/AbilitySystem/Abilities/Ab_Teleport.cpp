// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Teleport.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../_Misc/Helpers.h"

void UAb_Teleport::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (!PlayerPtr)
	{
		return;
	}

	// Set default teleport / cursor locations
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
	
	if (bShouldCrouch)
	{
		PlayerPtr->Crouch();
	}
	
	PlayerPtr->SetActorLocation(TeleportLocation);
}

void UAb_Teleport::Update(float _DeltaSeconds)
{
	if (!PlayerPtr)
	{
		return;
	}
	
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
}

void UAb_Teleport::OnDeactivation()
{
	NormalCursorPtr->Destroy();
	LedgeCursorPtr->Destroy();
	CrouchCursorPtr->Destroy();
}

void UAb_Teleport::GetTeleportVariables()
{
	bCanMantle = false;
	bShouldCrouch = false;
	
	FVector FinalTpLocation = TeleportLocation;
	FHitResult InitialTraceHit;

	const bool bInitialTrace = InitialTrace(FinalTpLocation, InitialTraceHit);
	
	if (bInitialTrace)
	{
		MantleTrace(InitialTraceHit, FinalTpLocation);
	}

	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Tp Location Before: %s"), *FinalTpLocation.ToString()));
	
	if (CheckHeadRoom(FinalTpLocation))
	{
		TeleportLocation = FinalTpLocation;
		CursorLocation = TeleportLocation - FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	}

	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Tp Location After: %s"), *FinalTpLocation.ToString()));
}

bool UAb_Teleport::InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit) const
{
	FVector TraceStart;
	FRotator TraceDirection;
	PlayerPtr->GetActorEyesViewPoint(TraceStart, TraceDirection);

	const bool bInitialTraceResult = PlayerPtr->GetWorld()->SweepSingleByChannel
	(
		_TraceHit,
		TraceStart,
		TraceStart + PlayerPtr->GetControlRotation().Vector() * TeleportRange,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InitialTraceRadius)
	);

	if (!bInitialTraceResult) // no trace hit, tp location is mid-air
	{
		_TeleportLocation = _TraceHit.TraceEnd - TraceDirection.Vector() * PlayerPtr->GetSimpleCollisionRadius()
								+ FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	}
	
	return bInitialTraceResult;
}

void UAb_Teleport::MantleTrace(const FHitResult& _InitialTraceHit, FVector& _TeleportLocation)
{
	const bool bMantleAble = FMath::Abs(FVector::DotProduct(FVector::UpVector, _InitialTraceHit.ImpactNormal)) <= WallDotTolerance;

	TArray<FHitResult> HitResultArr;
	
	if (bMantleAble)
	{
		FVector NormalLocalAxis[2];
		FHelpers::GetLocalAxisFromForward(_InitialTraceHit.ImpactNormal, NormalLocalAxis[0], NormalLocalAxis[1]);

		FHelpers::RecursiveSphereTrace
		(
				PlayerPtr,
				_InitialTraceHit.ImpactPoint,
				_InitialTraceHit.ImpactPoint,
				_InitialTraceHit.ImpactPoint + NormalLocalAxis[1] * EdgeTolerance * 2.f,
				MantleTraceRadius,
				HitResultArr
		);

		bCanMantle = FVector::Dist(_InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;
	}

	if (bCanMantle)
	{
		const float HeightPlusTolerance = PlayerPtr->GetSimpleCollisionHalfHeight() + WallTolerance * 2.f;
		_TeleportLocation = HitResultArr.Last().ImpactPoint + FVector::UpVector * HeightPlusTolerance;
	}
	else
	{
		const FVector ExtentPlusTolerance = PlayerPtr->GetSimpleCollisionCylinderExtent() + FVector(WallTolerance);
		_TeleportLocation = _InitialTraceHit.ImpactPoint + _InitialTraceHit.ImpactNormal * ExtentPlusTolerance;
	}
}

bool UAb_Teleport::CheckHeadRoom(FVector& _TeleportLocation)
{
	FVector DepenetrationVector;
	const FVector TpLocationNoOffset = _TeleportLocation;

	const float PlayerRadius = PlayerPtr->GetSimpleCollisionRadius();
	const float PlayerHalfHeight = PlayerPtr->GetSimpleCollisionHalfHeight();
	const float PlayerCrouchedHalfHeight = PlayerPtr->GetCharacterMovement()->CrouchedHalfHeight;
	
	bool bCanTeleport = FHelpers::CheckPlayerHeadRoom(PlayerPtr->GetWorld(), PlayerRadius, _TeleportLocation,
		PlayerHalfHeight, DepenetrationVector, DepenetrationPadding); // standing check #1
	
	if (!bCanTeleport)
	{
		_TeleportLocation += DepenetrationVector;
		bCanTeleport = FHelpers::CheckPlayerHeadRoom(PlayerPtr->GetWorld(), PlayerRadius, _TeleportLocation,
			PlayerHalfHeight, DepenetrationVector, DepenetrationPadding); // standing check #2

		if (!bCanTeleport && !PlayerPtr->bIsCrouched)
		{
			_TeleportLocation = TpLocationNoOffset - FVector::UpVector * (PlayerHalfHeight - PlayerCrouchedHalfHeight);
			bShouldCrouch = FHelpers::CheckPlayerHeadRoom(PlayerPtr->GetWorld(), PlayerRadius, _TeleportLocation,
				PlayerCrouchedHalfHeight, DepenetrationVector, DepenetrationPadding); // crouch check #1

			if (!bShouldCrouch)
			{
				_TeleportLocation += DepenetrationVector;
				bShouldCrouch = FHelpers::CheckPlayerHeadRoom(PlayerPtr->GetWorld(), PlayerRadius, _TeleportLocation,
					PlayerCrouchedHalfHeight, DepenetrationVector, DepenetrationPadding); // crouch check #2
			}

			bCanTeleport = bShouldCrouch;
		}
	}

	return bCanTeleport;
}
