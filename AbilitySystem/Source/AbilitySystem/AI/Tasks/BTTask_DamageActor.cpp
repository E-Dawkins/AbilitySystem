// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DamageActor.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DamageActor::UBTTask_DamageActor()
{
	NodeName = TEXT("Damage Actor");
	bNotifyTick = true;
	
	ActorToDamage.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DamageActor, ActorToDamage), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_DamageActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_DamageActor::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* Actor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorToDamage.SelectedKeyName));
	
	if (IsValid(Actor))
	{
		Actor->TakeDamage(DamagePerSecond * DeltaSeconds, FDamageEvent(), OwnerComp.GetOwner()->GetInstigatorController(), OwnerComp.GetOwner());
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
