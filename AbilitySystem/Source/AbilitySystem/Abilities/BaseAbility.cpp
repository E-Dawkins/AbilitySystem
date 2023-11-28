// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"

void UBaseAbility::OnActivation(UWorld* _World)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Base ability activated"));
}

void UBaseAbility::OnUse(APlayerCharacter* _Player)
{
	FString DebugMessage = "Base ability used, but called with Player == nullptr!";

	if (_Player)
	{
		DebugMessage = FString::Printf(TEXT("Base ability used by %s"), *_Player->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
	}
}

void UBaseAbility::Update(APlayerCharacter* _Player, float _DeltaSeconds)
{
	FString DebugMessage = "Base ability updated, but called with Player == nullptr!";

	if (_Player)
	{
		DebugMessage = FString::Printf(TEXT("Base ability updated by %s"), *_Player->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
	}
}

void UBaseAbility::OnDeactivation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Base ability de-activated"));
}
