// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Ab_BendTime.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_BendTime : public UBaseAbility
{
	GENERATED_BODY()

	virtual void OnActivation(APlayerCharacter* _Player) override;
	virtual void OnUse() override;
	virtual void Update(float _DeltaSeconds) override;

private:
	void ToggleTimeBend();
	
private:
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.0", ClampMax = "1.0"))
	float GlobalDilation = 0.3f;
	
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.0", ClampMax = "1.0"))
	float PlayerDilation = 1.f;

	// (seconds)
	UPROPERTY(EditAnywhere, Category = "BendTime", meta=(ClampMin = "0.1"))
	float BendTimeLength = 1.f;

	UPROPERTY(EditAnywhere, Category = "BendTime|Debugging")
	bool bDrawTimeRemaining = false;
	
private:
	float StoredGlobalDilation = 1.f;
	float StoredPlayerDilation = 1.f;

	bool bIsBendingTime = false;

	FTimerHandle BendTimeHandle = FTimerHandle();
	
};
