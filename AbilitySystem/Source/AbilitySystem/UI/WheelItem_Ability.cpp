// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelItem_Ability.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

UWheelItem_Ability::UWheelItem_Ability()
{
	// Don't edit icons from items' inspector directly, use ability's icons instead
	bHideIconsInInspector = true;
}

void UWheelItem_Ability::InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position,
                                        FVector2D _NormalSize, FVector2D _SelectedSize)
{
	if (IsValid(Ability))
	{
		NormalIcon = Ability.GetDefaultObject()->NormalIcon;
		SelectedIcon = Ability.GetDefaultObject()->SelectedIcon;
	}
	
	Super::InitializeItem(_WidgetTree, _ItemParent, _Position, _NormalSize, _SelectedSize);
}

void UWheelItem_Ability::ItemSelect()
{
	Super::ItemSelect();

	if (APlayerCharacter* Player = GetPlayerPtr())
	{
		Player->SetCurrentAbility(Ability);
	}
}
