// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BaseAbility.h"

#include "Ab_Blink.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class UAb_Blink : public UBaseAbility
{
	GENERATED_BODY()

protected:
	virtual void OnActivation(APlayerCharacter* _Player) override;
	virtual void Update(float _DeltaSeconds) override;
	virtual void OnDeactivation() override;
	virtual void OnUse() override;

	// Used to swap out a couple lines at the end of the OnUse function
	virtual void OnUseEnd();
	
	virtual void UpdateTeleportVariables(APlayerCharacter* _PlayerPtr);

	// Returns true if the trace hit something
	virtual bool InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit, APlayerCharacter* _PlayerPtr) const;
	virtual void MantleTrace(const FHitResult _InitialTraceHit, FVector& _TeleportLocation, APlayerCharacter* _PlayerPtr);

	// Returns true when the player has sufficient head room at teleport location
	virtual bool CheckHeadRoom(FVector& _TeleportLocation, APlayerCharacter* _PlayerPtr);

protected:
	UPROPERTY(EditAnywhere, Category = "Teleport|Cursors")
	TSubclassOf<AActor> NormalCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport|Cursors")
	TSubclassOf<AActor> LedgeCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport|Cursors")
	TSubclassOf<AActor> CrouchCursor;
	
	// (cm)
	UPROPERTY(EditAnywhere, Category = "Teleport", meta=(ClampMin="100.0"))
	float TeleportRange = 1000.f;

	// How far away from a top edge (cm) to teleport on top of it
	UPROPERTY(EditAnywhere, Category = "Teleport", meta=(ClampMin="10.0"))
	float EdgeTolerance = 50.f;

	// How steep before a wall is not counted as a wall, i.e. 0 => has to be exactly flat wall, 1 => the ground/ceiling counts as a wall
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WallDotTolerance = 0.6f;

	// The radius (cm) of the initial sphere trace
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.1"))
	float InitialTraceRadius = 25.f;

	// The radius (cm) of the mantle recursive sphere trace
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.1"))
	float MantleTraceRadius = 10.f;

	// The tolerance (cm) to offset the tp location by, when aiming at a wall / mantling
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.1"))
	float WallTolerance = 5.f;
	
	// The depenetration padding (cm) when running second pass of head-checks
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.1"))
	float DepenetrationPadding = 2.5f;

#pragma region Debugging
	
	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawStats = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawInitialTraceHit = false;
	
	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawHeadRoom = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawHeadRoom", EditConditionHides))
	FColor HeadRoomSucceed = FColor::Green;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging")
	bool bDrawMantle = false;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleLocalUp = FColor::Purple;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleTraceFail = FColor::Red;

	UPROPERTY(EditAnywhere, Category = "Teleport|Debugging", meta = (EditCondition = "bDrawMantle", EditConditionHides))
	FColor MantleTraceSucceed = FColor::Green;

#pragma endregion
	
protected:
	UPROPERTY()
	AActor* NormalCursorPtr;
	
	UPROPERTY()
	AActor* LedgeCursorPtr;
	
	UPROPERTY()
	AActor* CrouchCursorPtr;
	
	FVector TeleportLocation;
	FVector CursorLocation;

	bool bShouldCrouch;
	bool bCanMantle;
	
};
