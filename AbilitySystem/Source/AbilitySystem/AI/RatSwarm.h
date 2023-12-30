// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RatSwarm.generated.h"

class AAIController;
class UBehaviorTree;

UCLASS()
class ABILITYSYSTEM_API ARatSwarm : public ACharacter
{
	GENERATED_BODY()

public:
	ARatSwarm();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* StaticMesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior = nullptr;

private:
	UPROPERTY()
	AAIController* AIController = nullptr;

};
