// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseAbility.h"
#include "BendTime_Manager.h"
#include "Ab_BendTime.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_BendTime : public UBaseAbility
{
	GENERATED_BODY()

	virtual void ResetAbility() override;
	virtual void OnUse() override;
	
private:
	UFUNCTION()
	void ToggleTimeBend();
	
private:
	UPROPERTY(EditAnywhere, Category="BendTime")
	FTimeBendOptions Options;
	
private:
	bool bIsBendingTime = false;
	
};
