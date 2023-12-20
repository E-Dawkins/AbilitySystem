// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Ab_FarReach.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_FarReach : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivation(APlayerCharacter* _Player) override;
	virtual void OnUse() override;
	virtual void Update(float _DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	void UpdateTeleportVariables();

	// Returns true if the trace hit something
	bool InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit) const;
	void MantleTrace(const FHitResult _InitialTraceHit, FVector& _TeleportLocation);

	// Returns true when the player has sufficient head room at teleport location
	bool CheckHeadRoom(FVector& _TeleportLocation);

	void SweepTowards();

private:
	UPROPERTY(EditAnywhere, Category = "FarReach|Cursors")
	TSubclassOf<AActor> NormalCursor;

	UPROPERTY(EditAnywhere, Category = "FarReach|Cursors")
	TSubclassOf<AActor> LedgeCursor;

	UPROPERTY(EditAnywhere, Category = "FarReach|Cursors")
	TSubclassOf<AActor> CrouchCursor;

	// (cm)
	UPROPERTY(EditAnywhere, Category = "FarReach", meta=(ClampMin="100.0"))
	float TeleportRange = 1000.f;

	// How far away from a top edge (cm) to teleport on top of it
	UPROPERTY(EditAnywhere, Category = "FarReach", meta=(ClampMin="10.0"))
	float EdgeTolerance = 50.f;

	// How steep before a wall is not counted as a wall, i.e. 0 => has to be exactly flat wall, 1 => the ground/ceiling counts as a wall
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WallDotTolerance = 0.6f;

	// The radius (cm) of the initial sphere trace
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = "0.1"))
	float InitialTraceRadius = 25.f;

	// The radius (cm) of the mantle recursive sphere trace
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = "0.1"))
	float MantleTraceRadius = 10.f;

	// The tolerance (cm) to offset the tp location by, when aiming at a wall / mantling
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = "0.1"))
	float WallTolerance = 5.f;

	// The depenetration padding (cm) when running second pass of head-checks
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = "0.1"))
	float DepenetrationPadding = 2.5f;

	// The sweep size (cm), when moving towards the teleport location
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 5.f))
	float SweepStepSize = 50.f;

	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 0.f))
	float MinExitVelocity = 100.f;

	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 0.f))
	float MaxExitVelocity = 500.f;
	

#pragma region Debugging
	
	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging")
	bool bDrawStats = false;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging")
	bool bDrawInitialTraceHit = false;
	
	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging")
	bool bDrawHeadRoom = false;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomSucceed = FColor::Green;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging")
	bool bDrawMantle = false;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleLocalUp = FColor::Purple;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleTraceFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "FarReach|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleTraceSucceed = FColor::Green;

#pragma endregion

private:
	UPROPERTY()
	AActor* NormalCursorPtr;
	
	UPROPERTY()
	AActor* LedgeCursorPtr;
	
	UPROPERTY()
	AActor* CrouchCursorPtr;

	FVector TeleportLocation;
	FVector CursorLocation;
	FVector VelocityBeforeTp = FVector::ZeroVector;
	
	bool bCanMantle;
	bool bShouldCrouch;
	bool bIsSweeping;
	
};
