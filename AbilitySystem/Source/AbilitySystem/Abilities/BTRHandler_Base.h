// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTRHandler_Base.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBTRHandler_Base : public UObject
{
	GENERATED_BODY()

public:
	virtual void SetupHandle() {};
	virtual void StartHandle(UActorComponent* CompToHandle) {};
	virtual void EndHandle(UActorComponent* CompToHandle) {};
		
};
