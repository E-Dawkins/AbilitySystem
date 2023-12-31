// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyManager.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UEnemyManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void ManagerInit(const UWorld::FActorsInitializedParams& _Params);

	void GetClosestEnemy(FVector _Point, AActor*& _ClosestEnemy, float& _Distance);
	
private:
	UPROPERTY()
	TArray<AActor*> AllEnemies;
	
};
