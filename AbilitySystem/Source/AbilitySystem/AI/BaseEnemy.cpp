// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "Components/CapsuleComponent.h"

ABaseEnemy::ABaseEnemy()
{
	
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Health <= 0.f && !bIsDead)
	{
		OnDeath();
	}

	if (CorpseHealth <= 0.f && !bIsCorpseDead)
	{
		OnCorpseDeath();
	}
}

float ABaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Health -= DamageAmount;

	if (Health <= 0.f)
	{
		CorpseHealth -= DamageAmount;
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseEnemy::OnDeath()
{
	bIsDead = true;
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy::OnCorpseDeath()
{
	bIsCorpseDead = true;
	
	Destroy();
}

