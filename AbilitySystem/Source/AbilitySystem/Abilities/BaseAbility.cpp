// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

void UBaseAbility::OnActivation(APlayerCharacter* _Player)
{
	PlayerPtr = _Player;
	bAbilityActive = true;
}

void UBaseAbility::OnUse()
{
	
}

void UBaseAbility::Update(float _DeltaSeconds)
{
	
}

void UBaseAbility::OnDeactivation()
{
	bAbilityActive = false;
}
