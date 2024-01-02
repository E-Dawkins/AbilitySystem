// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetClosestEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTService_GetClosestEnemy : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetClosestEnemy();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// Object key to save closest enemy pointer into
	UPROPERTY(EditAnywhere, Category = "Get Closest Enemy")
	FBlackboardKeySelector EnemyActor;
	
};
