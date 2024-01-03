// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy_Character.h"

#include "Components/CapsuleComponent.h"

ABaseEnemy_Character::ABaseEnemy_Character()
{
	
}

void ABaseEnemy_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEnemy_Character::Tick(float DeltaSeconds)
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

float ABaseEnemy_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Health -= DamageAmount;

	if (Health <= 0.f)
	{
		CorpseHealth -= DamageAmount;
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseEnemy_Character::OnDeath()
{
	bIsDead = true;
	
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy_Character::OnCorpseDeath()
{
	bIsCorpseDead = true;
	
	Destroy();
}

