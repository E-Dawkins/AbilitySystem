// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRHandler_AI.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"

void UBTRHandler_AI::StartHandle(UActorComponent* CompToHandle)
{
	Super::StartHandle(CompToHandle);

	if (auto AICharacter = Cast<ACharacter>(CompToHandle->GetOwner()))
	{
		if (auto AIController = Cast<AAIController>(AICharacter->Controller))
		{
			if (auto BTComp = Cast<UBehaviorTreeComponent>(AIController->BrainComponent))
			{
				BTComp->StopTree();
			}
		}
	}
}

void UBTRHandler_AI::EndHandle(UActorComponent* CompToHandle)
{
	Super::EndHandle(CompToHandle);

	if (auto AICharacter = Cast<ACharacter>(CompToHandle->GetOwner()))
	{
		if (auto AIController = Cast<AAIController>(AICharacter->Controller))
		{
			if (auto BTComp = Cast<UBehaviorTreeComponent>(AIController->BrainComponent))
			{
				BTComp->RestartTree();
			}
		}
	}
}
