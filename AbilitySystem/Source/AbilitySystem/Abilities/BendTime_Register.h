// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BendTime_Register.generated.h"


struct FTimeBendOptions;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBendTime_Register : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	void OnStartTimeBend(FTimeBendOptions Options);
	void OnEndTimeBend(FTimeBendOptions Options);

private:
	friend class UBendTime_Manager;

	UPROPERTY(EditAnywhere)
	bool bIgnoreTimeBend = false;
	
};
