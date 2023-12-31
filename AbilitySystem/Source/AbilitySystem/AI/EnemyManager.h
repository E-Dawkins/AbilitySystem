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

	AActor* GetClosestEnemy(FVector _Point);
	
private:
	UPROPERTY()
	TArray<AActor*> AllEnemies;
	
};
