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
	virtual void OnActivation(UWorld* World) override;
	virtual void OnUse(APlayerCharacter* Player) override;
	virtual void Update(APlayerCharacter* Player, float DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	void GetTeleportVariables(APlayerCharacter* _Player);

	void GetAllSphereTraceHits(const UObject* WorldContextObject, const FVector OrigStart, const FVector Start, const FVector End, float Radius, ECollisionChannel TraceChannel,
		bool bTraceComplex, const TArray<AActor*>& IgnoredActors, EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, TArray<FHitResult>& OutHits, int MaxIterations = 10);

	bool FreeHeadRoom(APlayerCharacter* _Player, FVector _PlayerCenterAtNewLocation, float _PlayerHalfHeightToCheck);

private:
	UPROPERTY(EditAnywhere, Category = "Teleport")
	FTeleportCursor NormalCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FTeleportCursor LedgeCursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportRange = 500.f;

	// How far away from a top edge to teleport on top of it
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float EdgeTolerance = 50.f;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	bool bDebug = false;

	AActor* NormalCursorPtr;
	AActor* LedgeCursorPtr;

	FVector TeleportLocation;
	FVector CursorLocation;
	bool bCanTeleport;
	bool bCanMantle;
	bool bShouldCrouch;
	
};
