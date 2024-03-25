// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BendTime_Register_Handler_Base.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBendTime_Register_Handler_Base : public UObject
{
	GENERATED_BODY()

public:
	virtual void SetupHandle() {};
	virtual void StartHandle(UActorComponent* CompToHandle) {};
	virtual void EndHandle(UActorComponent* CompToHandle) {};
		
};
