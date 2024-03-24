// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register.h"
#include "BendTime_Manager.h"
#include "BendTime_Register_Handler_Base.h"
#include "Kismet/GameplayStatics.h"

void UBendTime_Register::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetOwner()->GetWorld())
	{
		World->GetSubsystem<UBendTime_Manager>()->RegisterComponent(this);
	}

	for (auto H : Handlers)
	{
		HandlerPointers.Add(NewObject<UBendTime_Register_Handler_Base>(this, H));
	}
}

void UBendTime_Register::OnStartTimeSlow(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
	{
		CustomDilation = GetOwner()->CustomTimeDilation;
		
		GetOwner()->CustomTimeDilation = 1.f / UGameplayStatics::GetGlobalTimeDilation(GetOwner()->GetWorld());
	}
}

void UBendTime_Register::OnEndTimeSlow(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
	{
		GetOwner()->CustomTimeDilation = CustomDilation;
	}
}

void UBendTime_Register::OnStartTimeStop(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
		return;
	
	bActorTickEnabled = GetOwner()->IsActorTickEnabled();
	bSimulatePhysics = GetOwner()->GetRootComponent()->IsSimulatingPhysics();

	for (const auto Handler : HandlerPointers)
	{
		Handler->StartHandle(GetOwner());
	}
	
	GetOwner()->SetActorTickEnabled(false);
	Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(false);
}

void UBendTime_Register::OnEndTimeStop(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
		return;
	
	GetOwner()->SetActorTickEnabled(bActorTickEnabled);
	Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(bSimulatePhysics);

	for (const auto Handler : HandlerPointers)
	{
		Handler->EndHandle(GetOwner());
	}
}
