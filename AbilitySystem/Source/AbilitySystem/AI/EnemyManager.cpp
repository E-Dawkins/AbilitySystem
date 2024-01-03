// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include "BaseEnemy_Character.h"
#include "Kismet/GameplayStatics.h"

void UEnemyManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Bind init function to run when the world has finished initializing
	GetWorld()->OnActorsInitialized.AddUObject(this, &UEnemyManager::ManagerInit);
}

void UEnemyManager::ManagerInit(const UWorld::FActorsInitializedParams& _Params)
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy_Character::StaticClass(), AllEnemies);
}

void UEnemyManager::GetClosestEnemy(FVector _Point, AActor*& _ClosestEnemy, float& _Distance)
{
	_ClosestEnemy = nullptr;
	_Distance = -1.f;
	
	for (AActor* Enemy : AllEnemies)
	{
		if (!IsValid(Enemy))
		{
			continue;
		}
		
		const float CurDist = FVector::Dist(_Point, Enemy->GetActorLocation());

		if (!IsValid(_ClosestEnemy) || CurDist < _Distance)
		{
			_ClosestEnemy = Enemy;
			_Distance = CurDist;
		}
	}
}
