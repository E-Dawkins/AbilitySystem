// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Ab_Teleport.generated.h"

USTRUCT(Atomic)
struct FTeleportCursor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CursorClass;

	UPROPERTY(EditAnywhere)
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere)
	FRotator Rotation = FRotator::ZeroRotator;
};

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_Teleport : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivation(UWorld* _World) override;
	virtual void OnUse(APlayerCharacter* _Player) override;
	virtual void Update(APlayerCharacter* _Player, float _DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	void GetTeleportVariables(APlayerCharacter* _Player);

	void RecursiveSphereTrace(const UObject* _WorldContextObject, const FVector _OrigStart, const FVector _Start, const FVector _End, float _Radius, ECollisionChannel _TraceChannel,
		bool _bTraceComplex, const TArray<AActor*>& _IgnoredActors, EDrawDebugTrace::Type _DrawDebugType, bool _bIgnoreSelf, TArray<FHitResult>& _OutHits, int _MaxIterations = 10);

	bool FreeHeadRoom(APlayerCharacter* _Player, FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck, FHitResult& _OverlapHit);

	FVector GetUpFromForward(FVector _Forward);

private:
	UPROPERTY(EditAnywhere, Category = "Teleport")
	FTeleportCursor NormalCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FTeleportCursor LedgeCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FTeleportCursor CrouchCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportRange = 500.f;

	// How far away from a top edge to teleport on top of it
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float EdgeTolerance = 50.f;

	// How steep before a wall is not counted as a wall
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WallDotTolerance = 0.15f;

#pragma region Debugging

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawStats = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawHeadRoom = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomSucceed = FColor::Green;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawMantle = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleDirection = FColor::Purple;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor SphereTraceFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor SphereTraceSucceed = FColor::Green;


#pragma endregion

private:
	AActor* NormalCursorPtr;
	AActor* LedgeCursorPtr;
	AActor* CrouchCursorPtr;

	FVector TeleportLocation;
	FVector CursorLocation;
	bool bCanTeleport;
	bool bCanMantle;
	bool bShouldCrouch;
	
};
