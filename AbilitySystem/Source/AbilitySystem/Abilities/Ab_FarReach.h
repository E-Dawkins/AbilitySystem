// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ab_Blink.h"
#include "Ab_FarReach.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_FarReach : public UAb_Blink
{
	GENERATED_BODY()

public:
	virtual void OnUseEnd() override;
	virtual void Update(float _DeltaSeconds) override;

	virtual bool InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit, APlayerCharacter* _PlayerPtr) const override;

private:
	void SweepTowards();

private:
	// The sweep size (cm), when moving towards the teleport location
	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 5.f))
	float SweepStepSize = 50.f;

	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 0.f))
	float MinExitVelocity = 100.f;

	UPROPERTY(EditAnywhere, Category = "FarReach", meta = (ClampMin = 0.f))
	float MaxExitVelocity = 500.f;

private:
	FVector VelocityBeforeTp = FVector::ZeroVector;
	
	bool bIsSweeping;
	
};
