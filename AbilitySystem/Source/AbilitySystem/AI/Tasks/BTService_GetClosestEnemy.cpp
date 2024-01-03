// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_GetClosestEnemy.h"

#include "AbilitySystem/AI/BaseEnemy_Character.h"
#include "AbilitySystem/AI/EnemyManager.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_GetClosestEnemy::UBTService_GetClosestEnemy()
{
	NodeName = TEXT("Get Closest Enemy");
	
	EnemyActor.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetClosestEnemy, EnemyActor), AActor::StaticClass());
}

void UBTService_GetClosestEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* ClosestEnemy;
	float Distance;
	GetWorld()->GetSubsystem<UEnemyManager>()->GetClosestEnemy(OwnerComp.GetOwner()->GetActorLocation(), ClosestEnemy, Distance);

	if (IsValid(ClosestEnemy))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(EnemyActor.SelectedKeyName, ClosestEnemy);
	}
}
