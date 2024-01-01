// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DamageActor.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTTask_DamageActor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DamageActor();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Damage Actor")
	FBlackboardKeySelector ActorToDamage;

	UPROPERTY(EditAnywhere, Category = "Damage Actor")
	float DamagePerSecond = 33;
	
};
