// Fill out your copyright notice in the Description page of Project Settings.


#include "RatSwarm.h"

#include "AIController.h"
#include "EnemyManager.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ARatSwarm::ARatSwarm() {}

void ARatSwarm::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(Controller);

	if (IsValid(AIController))
	{
		if (IsValid(AIBehavior))
		{
			AIController->RunBehaviorTree(AIBehavior);

			AActor* ClosestEnemy = GetWorld()->GetSubsystem<UEnemyManager>()->GetClosestEnemy(GetActorLocation());

			if (IsValid(ClosestEnemy))
			{
				AIController->GetBlackboardComponent()->SetValueAsVector(TEXT("VectorKey"), ClosestEnemy->GetActorLocation());
			}
		}
	}
}
