// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTRHandler_Base.h"
#include "BTRHandler_AI.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTRHandler_AI : public UBTRHandler_Base
{
	GENERATED_BODY()

	virtual void StartHandle(UActorComponent* CompToHandle) override;
	virtual void EndHandle(UActorComponent* CompToHandle) override;
	
};
