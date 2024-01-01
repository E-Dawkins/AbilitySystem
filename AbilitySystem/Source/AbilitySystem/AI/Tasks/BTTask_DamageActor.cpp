// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DamageActor.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DamageActor::UBTTask_DamageActor()
{
	NodeName = TEXT("Damage Actor");

	ActorToDamage.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DamageActor, ActorToDamage), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_DamageActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorToDamage.SelectedKeyName));

	if (IsValid(Actor))
	{
		Actor->TakeDamage(DamageAmount, FDamageEvent(), OwnerComp.GetOwner()->GetInstigatorController(), OwnerComp.GetOwner());
	}
	
	return Result;
}
