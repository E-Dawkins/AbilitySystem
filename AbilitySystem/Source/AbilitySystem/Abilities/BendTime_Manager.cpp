// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Manager.h"

#include "BendTime_Register.h"
#include "Kismet/GameplayStatics.h"

void UBendTime_Manager::OnStartTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		GlobalDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
		
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Options.GlobalDilationAmount);
	}
	
	for (const auto Comp : RegisteredComps)
	{
		switch (Options.TimeBendType)
		{
			case TB_SLOW:	Comp->OnStartTimeSlow(Options); break;
			default:		Comp->OnStartTimeStop(Options); break;
		}
	}
}

void UBendTime_Manager::OnStopTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalDilation);
	}
	
	for (const auto Comp : RegisteredComps)
	{
		switch (Options.TimeBendType)
		{
			case TB_SLOW:	Comp->OnEndTimeSlow(Options); break;
			default:		Comp->OnEndTimeStop(Options); break;
		}
	}
}

void UBendTime_Manager::RegisterComponent(UBendTime_Register* CompToRegister)
{
	RegisteredComps.Add(CompToRegister);
}
