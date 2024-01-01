// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetClosestEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTTask_GetClosestEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GetClosestEnemy();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// Enemy location to save into
	UPROPERTY(EditAnywhere, Category = "Get Closest Enemy")
	FBlackboardKeySelector EnemyLocation;

	UPROPERTY(EditAnywhere, Category = "Get Closest Enemy")
	FBlackboardKeySelector EnemyActor;
	
};
