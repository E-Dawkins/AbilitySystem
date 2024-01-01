// Fill out your copyright notice in the Description page of Project Settings.


#include "RatSwarm.h"

#include "AIController.h"
#include "BaseEnemy.h"
#include "EnemyManager.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ARatSwarm::ARatSwarm()
{
	RatSwarmSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Rat Swarm System"));
	RatSwarmSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ARatSwarm::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(Controller);

	if (IsValid(AIController))
	{
		if (IsValid(AIBehavior))
		{
			AIController->RunBehaviorTree(AIBehavior);
		}
	}
}

void ARatSwarm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// if (Distance <= DamageRadius)
	// {
	// 	ClosestEnemy->TakeDamage(EnemyDamagePerSecond * DeltaSeconds, FDamageEvent(), AIController, this);
	// }
}
