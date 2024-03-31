// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTRHandler_Base.h"
#include "BTRHandler_Particles.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UBTRHandler_Particles : public UBTRHandler_Base
{
	GENERATED_BODY()

public:
	virtual void StartHandle(UActorComponent* CompToHandle) override;
	virtual void EndHandle(UActorComponent* CompToHandle) override;
	
};
