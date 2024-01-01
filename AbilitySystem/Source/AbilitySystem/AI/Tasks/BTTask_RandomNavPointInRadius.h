// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomNavPointInRadius.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTTask_RandomNavPointInRadius : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RandomNavPointInRadius();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Random Nav Point In Radius")
	FBlackboardKeySelector NavPoint;
	
	UPROPERTY(EditAnywhere, Category = "Random Nav Point In Radius")
	float Radius = 500.f;
	
};
