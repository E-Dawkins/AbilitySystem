// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Manager.h"

#include "BendTime_Register.h"
#include "Kismet/GameplayStatics.h"

void UBendTime_Manager::OnStartTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), Options.GlobalDilationAmount);
	}
	
	for (const auto Comp : RegisteredComps)
	{
		Comp->OnStartTimeBend(Options);
	}
}

void UBendTime_Manager::OnStopTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	}
	
	for (const auto Comp : RegisteredComps)
	{
		Comp->OnEndTimeBend(Options);
	}
}

void UBendTime_Manager::RegisterComponent(UBendTime_Register* CompToRegister)
{
	RegisteredComps.Add(CompToRegister);
}
