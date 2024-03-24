// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BendTime_Register_Handler_Base.h"
#include "BendTime_Register_Handler_Meshes.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBendTime_Register_Handler_Meshes : public UBendTime_Register_Handler_Base
{
	GENERATED_BODY()

public:
	virtual void StartHandle(AActor* ActorToHandle) override;
	virtual void EndHandle(AActor* ActorToHandle) override;

private:
	UPROPERTY()
	TMap<UMeshComponent*, FVector> Velocities;
	
};
