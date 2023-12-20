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

	UWheelItem_Ability();

public:
	virtual void InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position, FVector2D _NormalSize, FVector2D _SelectedSize) override;
	virtual void ItemSelect() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBaseAbility> Ability;
	
};
