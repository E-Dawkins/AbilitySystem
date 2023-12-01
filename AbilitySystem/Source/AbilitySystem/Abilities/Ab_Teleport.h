// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Ab_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_Teleport : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivation(APlayerCharacter* _Player) override;
	virtual void OnUse() override;
	virtual void Update(float _DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	void GetTeleportVariables();

	void RecursiveSphereTrace(const FVector _OrigStart, const FVector _Start, const FVector _End,
		const float _Radius, TArray<FHitResult>& _OutHits, int _MaxIterations = 10);

	bool FreeHeadRoom(FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck, FVector& _DepenetrationVector) const;

	static FVector GetUpFromForward(FVector _Forward);

private:
	UPROPERTY(EditAnywhere, Category = "Teleport")
	TSubclassOf<AActor> NormalCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	TSubclassOf<AActor> LedgeCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	TSubclassOf<AActor> CrouchCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportRange = 1000.f;

	// How far away from a top edge to teleport on top of it
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float EdgeTolerance = 50.f;

	// How steep before a wall is not counted as a wall
	UPROPERTY(EditAnywhere, Category = "Teleport", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WallDotTolerance = 0.6f;

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
	UPROPERTY()
	AActor* NormalCursorPtr;
	
	UPROPERTY()
	AActor* LedgeCursorPtr;
	
	UPROPERTY()
	AActor* CrouchCursorPtr;

	FVector TeleportLocation;
	FVector CursorLocation;
	bool bCanMantle;
	bool bShouldCrouch;
	
};
