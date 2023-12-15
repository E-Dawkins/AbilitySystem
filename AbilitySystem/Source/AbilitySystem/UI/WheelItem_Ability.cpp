// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelItem_Ability.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

void UWheelItem_Ability::ItemSelect()
{
	Super::ItemSelect();

	if (APlayerCharacter* Player = GetPlayerPtr())
	{
		Player->SetCurrentAbility(Ability);
	}
}
