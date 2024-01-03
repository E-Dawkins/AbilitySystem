// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy_Character.generated.h"

UCLASS()
class ABILITYSYSTEM_API ABaseEnemy_Character : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy_Character();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void OnDeath();
	void OnCorpseDeath();

public:
	bool IsAlive() const { return Health > 0.f; }

private:
	UPROPERTY(EditAnywhere, Category = "Base Enemy")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Base Enemy")
	float CorpseHealth = 50.f;

	bool bIsDead = false;
	bool bIsCorpseDead = false;

};
