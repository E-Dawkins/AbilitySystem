// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponWheelItem.h"
#include "WheelItem_Ability.generated.h"

class UBaseAbility;
/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UWheelItem_Ability : public UWeaponWheelItem
{
	GENERATED_BODY()

public:
	virtual void ItemSelect() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBaseAbility> Ability;
	
};
