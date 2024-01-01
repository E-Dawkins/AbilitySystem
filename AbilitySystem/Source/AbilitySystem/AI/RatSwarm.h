// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RatSwarm.generated.h"

class UNiagaraComponent;
class AAIController;
class UBehaviorTree;

UCLASS(HideCategories = (Character,Camera,Replication,Rendering,Collision,Actor,Input,LOD,Cooking))
class ABILITYSYSTEM_API ARatSwarm : public ACharacter
{
	GENERATED_BODY()

public:
	ARatSwarm();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	UNiagaraComponent* RatSwarmSystem = nullptr;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior = nullptr;

	UPROPERTY(EditAnywhere)
	float EnemyDamagePerSecond = 20.f;

	UPROPERTY(EditAnywhere)
	float DamageRadius = 150.f;

private:
	UPROPERTY()
	AAIController* AIController = nullptr;

};
