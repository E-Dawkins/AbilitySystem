// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RatSwarm_Character.generated.h"

class UNiagaraComponent;
class AAIController;
class UBehaviorTree;

UCLASS(HideCategories = (Character,Camera,Replication,Rendering,Collision,Actor,Input,LOD,Cooking))
class ABILITYSYSTEM_API ARatSwarm_Character : public ACharacter
{
	GENERATED_BODY()

public:
	ARatSwarm_Character();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	UNiagaraComponent* RatSwarmSystem = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	UNiagaraComponent* BloodSpraySystem = nullptr;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior = nullptr;

	// The blackboard entry to get the enemy actor from.
	UPROPERTY(EditAnywhere)
	FString EnemyBlackboardKeyName = "EnemyActor";

	// When a corpse is within this radius, activate the blood spray system.
	UPROPERTY(EditAnywhere)
	float BloodActivationRadius = 100.f;

	// In Seconds
	UPROPERTY(EditAnywhere)
	float SwarmLength = 10.f;

	UPROPERTY(EditAnywhere)
	FString SwarmLengthVariable = "SwarmLength";

private:
	UPROPERTY()
	AAIController* AIController = nullptr;

};
