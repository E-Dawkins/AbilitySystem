// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomNavPointInRadius.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RandomNavPointInRadius::UBTTask_RandomNavPointInRadius()
{
	NodeName = TEXT("Random Nav Point In Radius");

	NavPoint.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_RandomNavPointInRadius, NavPoint));
}

EBTNodeResult::Type UBTTask_RandomNavPointInRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const FVector Origin = OwnerComp.GetOwner()->GetActorLocation();

	FNavLocation RandomPoint;
	const bool bLocationExists = NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomPoint);

	if (bLocationExists)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(NavPoint.SelectedKeyName, RandomPoint.Location);
	}
	else
	{
		Result = EBTNodeResult::Failed;
	}
	
	return Result;
}
