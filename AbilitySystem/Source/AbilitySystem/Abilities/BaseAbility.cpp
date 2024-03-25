// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

UBaseAbility::UBaseAbility()
{
	FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UBaseAbility::BeginPlay);
	FWorldDelegates::OnPostWorldCleanup.AddUObject(this, &UBaseAbility::EndPlay);
}

void UBaseAbility::BeginPlay(UWorld* World)
{
	ResetAbility();
}

void UBaseAbility::EndPlay(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	ResetAbility();
}

void UBaseAbility::ResetAbility()
{
	PlayerPtr = nullptr;
	bAbilityActive = false;
}

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

void UBaseAbility::BeginDestroy()
{
	UObject::BeginDestroy();

	OnDeactivation();
}
