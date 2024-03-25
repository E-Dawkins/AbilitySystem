// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BendTime_Register_Handler_Base.h"
#include "BendTime_Register_Handler_Meshes.generated.h"

USTRUCT(Atomic)
struct FMeshData
{
	GENERATED_BODY()
	
	FVector LinearVelocity = FVector::ZeroVector;
	FVector AngularVelocity = FVector::ZeroVector;
};

UCLASS()
class ABILITYSYSTEM_API UBendTime_Register_Handler_Meshes : public UBendTime_Register_Handler_Base
{
	GENERATED_BODY()

public:
	virtual void SetupHandle() override;
	virtual void StartHandle(UActorComponent* CompToHandle) override;
	virtual void EndHandle(UActorComponent* CompToHandle) override;

private:
	UPROPERTY()
	TMap<UMeshComponent*, FMeshData> StoredData;
	
};
