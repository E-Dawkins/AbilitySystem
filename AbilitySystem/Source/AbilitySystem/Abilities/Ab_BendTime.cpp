// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_BendTime.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

void UAb_BendTime::ResetAbility()
{
	bIsBendingTime = false;
}

void UAb_BendTime::OnUse()
{
	Super::OnUse();
	
	if (!PlayerPtr)
	{
		return;
	}
	
	ToggleTimeBend();
}

void UAb_BendTime::ToggleTimeBend()
{
	bIsBendingTime = !bIsBendingTime;

	if (UBendTime_Manager* BTManager = PlayerPtr->GetWorld()->GetSubsystem<UBendTime_Manager>())
	{
		if (!bIsBendingTime)
		{
			BTManager->OnStopTimeBend(Options);
		}
		else
		{
			BTManager->OnStartTimeBend(Options);
		}
	}

	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, bIsBendingTime ? FColor::Green : FColor::Red, "Bending Time");
	}
}
