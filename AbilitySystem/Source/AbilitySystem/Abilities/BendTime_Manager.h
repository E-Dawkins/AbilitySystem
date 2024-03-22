// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BendTime_Manager.generated.h"

class UBendTime_Register;

UENUM()
enum ETimeBendType
{
	TB_SLOW = 0,
	TB_STOP
};

USTRUCT(Atomic)
struct FTimeBendOptions
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETimeBendType> TimeBendType = TB_SLOW;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0.1"))
	float GlobalDilationAmount = 0.5f;
};

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBendTime_Manager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void OnStartTimeBend(FTimeBendOptions Options);
	void OnStopTimeBend(FTimeBendOptions Options);

private:
	void RegisterComponent(UBendTime_Register* CompToRegister);

private:
	friend UBendTime_Register;
	
	UPROPERTY()
	TArray<UBendTime_Register*> RegisteredComps;
	
};
