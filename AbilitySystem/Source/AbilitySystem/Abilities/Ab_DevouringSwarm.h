// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Ab_DevouringSwarm.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_DevouringSwarm : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivation(APlayerCharacter* _Player) override;
	virtual void OnUse() override;
	virtual void Update(float _DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	bool GetTraceLocation(FVector& _TraceLocation) const;

private:
	UPROPERTY(EditAnywhere, Category = "Devouring Swarm")
	TSubclassOf<AActor> SwarmCursor;

	UPROPERTY(EditAnywhere, Category = "Devouring Swarm")
	TSubclassOf<AActor> Swarm;

	// The reach that the player can spawn the swarm (cm)
	UPROPERTY(EditAnywhere, Category = "Devouring Swarm")
	float CastReach = 500.f;

	UPROPERTY(EditAnywhere, Category = "Devouring Swarm")
	float SphereCastRadius = 25.f;

private:
	UPROPERTY()
	AActor* SwarmCursorPtr;

	UPROPERTY()
	AActor* SwarmPtr;
	
};
