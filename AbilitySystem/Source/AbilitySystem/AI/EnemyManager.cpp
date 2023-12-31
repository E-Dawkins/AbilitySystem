// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include "BaseEnemy.h"
#include "Kismet/GameplayStatics.h"

void UEnemyManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Bind init function to run when the world has finished initializing
	GetWorld()->OnActorsInitialized.AddUObject(this, &UEnemyManager::ManagerInit);
}

void UEnemyManager::ManagerInit(const UWorld::FActorsInitializedParams& _Params)
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), AllEnemies);
}

AActor* UEnemyManager::GetClosestEnemy(FVector _Point)
{
	AActor* Closest = nullptr;
	float ClosestDist = 0.f;
	
	for (AActor* Enemy : AllEnemies)
	{
		const float CurDist = FVector::Dist(_Point, Enemy->GetActorLocation());

		if (!IsValid(Closest) || CurDist < ClosestDist)
		{
			Closest = Enemy;
			ClosestDist = CurDist;
		}
	}

	return Closest;
}
