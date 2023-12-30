// Fill out your copyright notice in the Description page of Project Settings.


#include "RatSwarm.h"

#include "AIController.h"
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

			AIController->GetBlackboardComponent()->SetValueAsVector(TEXT("VectorKey"), GetActorLocation() + FVector(0, 5000, 0));
		}
	}
}
