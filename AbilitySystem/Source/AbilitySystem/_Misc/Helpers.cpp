// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers.h"
#include "DrawDebugHelpers.h"
#include "../Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void FHelpers::GetLocalAxisFromForward(FVector _Forward, FVector& _Right, FVector& _Up)
{
	_Right = FVector::CrossProduct(_Forward.GetSafeNormal(), FVector::UpVector).GetSafeNormal();
	_Up = FVector::CrossProduct(_Right, _Forward.GetSafeNormal());
}

void FHelpers::GetLocalAxisFromRight(FVector _Right, FVector& _Up, FVector& _Forward)
{
	_Up = FVector::CrossProduct(_Right.GetSafeNormal(), FVector::ForwardVector).GetSafeNormal();
	_Forward = FVector::CrossProduct(_Up, _Right.GetSafeNormal());
}

void FHelpers::GetLocalAxisFromUp(FVector _Up, FVector& _Forward, FVector& _Right)
{
	_Forward = FVector::CrossProduct(_Up.GetSafeNormal(), FVector::RightVector);
	_Right = FVector::CrossProduct(_Forward, _Up.GetSafeNormal());
}

void FHelpers::RecursiveSphereTrace(const UObject* _WorldContextObject, const FVector _OrigStart, const FVector _Start, const FVector _End,
	const float _Radius, TArray<FHitResult>& _OutHits, int _MaxIterations, const FTraceHelper _Helper)
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
		UEngineTypes::ConvertToTraceType(_Helper.CollisionChannel),
		_Helper.bTraceComplex,
		_Helper.IgnoredActors,
		_Helper.DrawDebugTrace,
		Hit,
		_Helper.bIgnoreSelf
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

		// Re-call this function, with offset starting location
		RecursiveSphereTrace(_WorldContextObject, _OrigStart, Hit.ImpactPoint + Direction * _Radius * 2.f, _End, _Radius, _OutHits, _MaxIterations, _Helper);
	}
}

bool FHelpers::CheckPlayerHeadRoom(const class APlayerCharacter* _PlayerPtr, const FVector _PlayerCenter, const EPlayerState _PlayerState, FDebugHelper _DebugHelper, FVector& _DepenetrationVector, const float _DepenetrationPadding, const ECollisionChannel _CollisionChannel)
{
	if (!_PlayerPtr)
	{
		return false;
	}
	
	FHitResult Hit;

	FCollisionQueryParams QueryParams;
	QueryParams.bFindInitialOverlaps = true; // life-saver, makes the depenetration vector work when trace starts penetrated

	const float PlayerRadius = _PlayerPtr->GetSimpleCollisionRadius();
	float PlayerHalfHeight = 0.f;

	switch(_PlayerState)
	{
		case Standing: PlayerHalfHeight = _PlayerPtr->GetSimpleCollisionHalfHeight(); break;
		case Crouched: PlayerHalfHeight = _PlayerPtr->GetCharacterMovement()->CrouchedHalfHeight; break;
		default: PlayerHalfHeight = 0.f;
	}
	
	const bool ObjectBlocking = _PlayerPtr->GetWorld()->SweepSingleByChannel
	(
		Hit,
		_PlayerCenter,
		_PlayerCenter,
		FQuat::Identity,
		_CollisionChannel,
		FCollisionShape::MakeCapsule(FVector(PlayerRadius, PlayerRadius, PlayerHalfHeight)),
		QueryParams
	);

	_DepenetrationVector = Hit.Normal * (Hit.PenetrationDepth + _DepenetrationPadding);

	if (_DebugHelper.bDebug)
	{
		DrawDebugCapsule(_PlayerPtr->GetWorld(), _PlayerCenter, PlayerHalfHeight, PlayerRadius, FQuat::Identity,
			ObjectBlocking ? _DebugHelper.Color_Fail : _DebugHelper.Color_Success);
	}

	return !ObjectBlocking;
}
