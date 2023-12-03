// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Helpers.generated.h"

USTRUCT(Atomic)
struct FTraceHelper
{
	GENERATED_BODY()
	
	bool bTraceComplex = false;
	bool bIgnoreSelf = true;
	
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Visibility;
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugTrace = EDrawDebugTrace::None;
	
	UPROPERTY()
	TArray<AActor*> IgnoredActors = TArray<AActor*>();
};

/**
 * 
 */
class ABILITYSYSTEM_API FHelpers
{
public:
	FHelpers() = default;
	~FHelpers() = default;
	
	// _Forward does not need to be normalized, it is done locally within this function.
	static void GetLocalAxisFromForward(FVector _Forward, FVector& _Right, FVector& _Up);

	// _Right does not need to be normalized, it is done locally within this function.
	static void GetLocalAxisFromRight(FVector _Right, FVector& _Up, FVector& _Forward);

	// _Up does not need to be normalized, it is done locally within this function.
	static void GetLocalAxisFromUp(FVector _Up, FVector& _Forward, FVector& _Right);

	static void RecursiveSphereTrace(const UObject* _WorldContextObject, const FVector _OrigStart, const FVector _Start, const FVector _End,
		const float _Radius, TArray<FHitResult>& _OutHits, int _MaxIterations = 10, const FTraceHelper& _Helper = FTraceHelper());

	// Gets the depenetration vector, if one exists, that will move the player out of collision
	static bool CheckPlayerHeadRoom(const UWorld* _WorldPtr, float _PlayerRadius, FVector _PlayerCenter,
		float _PlayerHalfHeight, FVector& _DepenetrationVector, float _DepenetrationPadding = 5.f, ECollisionChannel CollisionChannel = ECC_Visibility);
};
