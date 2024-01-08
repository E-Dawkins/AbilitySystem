// Fill out your copyright notice in the Description page of Project Settings.


#include "RatSwarm_Character.h"

#include "AIController.h"
#include "BaseEnemy_Character.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ARatSwarm_Character::ARatSwarm_Character()
{
	RatSwarmSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Rat Swarm System"));
	RatSwarmSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RatSwarmSystem->SetAutoActivate(false);

	BloodSpraySystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Blood Spray System"));
	BloodSpraySystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BloodSpraySystem->SetAutoActivate(false);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARatSwarm_Character::BeginPlay()
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

	RatSwarmSystem->SetFloatParameter(*SwarmLengthVariable, SwarmLength);
	RatSwarmSystem->Activate();
}

void ARatSwarm_Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	const ABaseEnemy_Character* Enemy = Cast<ABaseEnemy_Character>(AIController->GetBlackboardComponent()->GetValueAsObject(*EnemyBlackboardKeyName));

	const FVector SwarmLocation = RatSwarmSystem->GetComponentLocation();
	const FVector EnemyLocation = IsValid(Enemy) ? Enemy->GetActorLocation() : SwarmLocation;
	
	if (IsValid(Enemy) && !Enemy->IsAlive() && FVector::Dist(SwarmLocation, EnemyLocation) <= BloodActivationRadius)
	{
		BloodSpraySystem->SetUsingAbsoluteLocation(true);
		BloodSpraySystem->SetWorldLocation(FVector(EnemyLocation.X, EnemyLocation.Y, SwarmLocation.Z));

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

	if (RatSwarmSystem->GetSystemInstanceController() && RatSwarmSystem->GetSystemInstanceController()->GetAge() >= SwarmLength)
	{
		Destroy();
	}
}
