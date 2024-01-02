// Fill out your copyright notice in the Description page of Project Settings.


#include "RatSwarm.h"

#include "AIController.h"
#include "BaseEnemy.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ARatSwarm::ARatSwarm()
{
	RatSwarmSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Rat Swarm System"));
	RatSwarmSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	BloodSpraySystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Blood Spray System"));
	BloodSpraySystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BloodSpraySystem->SetAutoActivate(false);
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

	const ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetBlackboardComponent()->GetValueAsObject(*EnemyBlackboardKeyName));
	
	if (IsValid(Enemy) && !Enemy->IsAlive())
	{
		if (!BloodSpraySystem->IsActive())
		{
			BloodSpraySystem->Activate();
		}
	}
	else
	{
		if (BloodSpraySystem->IsActive())
		{
			BloodSpraySystem->Deactivate();
		}
	}
}
