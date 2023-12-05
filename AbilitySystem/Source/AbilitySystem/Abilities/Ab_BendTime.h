// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseAbility.h"
#include "Ab_BendTime.generated.h"

USTRUCT()
struct FFrozenActorData
{
	GENERATED_BODY()

	FBodyInstance BodyInstance;
	FVector Velocity;
};

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_BendTime : public UBaseAbility
{
	GENERATED_BODY()

	void OnActivation(APlayerCharacter* _Player) override;
	void OnUse() override;
	void Update(float _DeltaSeconds) override;

private:
	void ToggleTimeBend();
	void StartTimeBend(FVector _Velocity) const;
	void GetActorsCloseToPlayer();
	
private:
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.0001", ClampMax = "1.0"))
	float GlobalDilation = 0.3f;
	
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.0001", ClampMax = "1.0"))
	float PlayerDilation = 1.f;

	// (seconds)
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.1"))
	float BendTimeLength = 3.f;

	// If the global dilation is <= than this, treat it as time being "stopped"
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.0"))
	float TimeStopThreshold = 0.1f;

	UPROPERTY(EditAnywhere, Category = "BendTime|Debugging")
	bool bDrawTimeRemaining = false;
	
private:
	float StoredGlobalDilation = 1.f;
	float StoredPlayerDilation = 1.f;

	bool bIsBendingTime = false;
	
	FDateTime TimerStart = FDateTime::Now();
	
	TMap<AActor*, FFrozenActorData> ActorsCloseToPlayer;
	
};
