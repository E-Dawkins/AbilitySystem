// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register.h"
#include "BendTime_Manager.h"
#include "Kismet/GameplayStatics.h"

void UBendTime_Register::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetOwner()->GetWorld())
	{
		World->GetSubsystem<UBendTime_Manager>()->RegisterComponent(this);
	}
}

void UBendTime_Register::OnStartTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		if (bIgnoreTimeBend)
		{
			GetOwner()->CustomTimeDilation = 1.f / UGameplayStatics::GetGlobalTimeDilation(GetOwner()->GetWorld());
		}
	}
}

void UBendTime_Register::OnEndTimeBend(const FTimeBendOptions Options)
{
	if (Options.TimeBendType == TB_SLOW)
	{
		if (bIgnoreTimeBend)
		{
			GetOwner()->CustomTimeDilation = 1.f;
		}
	}
}
