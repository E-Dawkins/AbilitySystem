// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetClosestEnemy.h"

#include "AbilitySystem/AI/BaseEnemy.h"
#include "AbilitySystem/AI/EnemyManager.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetClosestEnemy::UBTTask_GetClosestEnemy()
{
	NodeName = TEXT("Get Closest Enemy");

	EnemyLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetClosestEnemy, EnemyLocation));
	EnemyActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetClosestEnemy, EnemyActor), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_GetClosestEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* ClosestEnemy;
	float Distance;
	GetWorld()->GetSubsystem<UEnemyManager>()->GetClosestEnemy(OwnerComp.GetOwner()->GetActorLocation(), ClosestEnemy, Distance);

	if (IsValid(ClosestEnemy))
	{
		if (Cast<ABaseEnemy>(ClosestEnemy)->IsAlive())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(EnemyLocation.SelectedKeyName, ClosestEnemy->GetActorLocation());
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(EnemyActor.SelectedKeyName, ClosestEnemy);
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
	return Result;
}
