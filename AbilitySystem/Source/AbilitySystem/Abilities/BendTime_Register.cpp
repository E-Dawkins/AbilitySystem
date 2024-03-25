// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register.h"
#include "BendTime_Manager.h"
#include "BendTime_Register_Handler_Base.h"
#include "BendTime_Register_Handler_Meshes.h"
#include "Kismet/GameplayStatics.h"

void UBendTime_Register::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetOwner()->GetWorld())
	{
		World->GetSubsystem<UBendTime_Manager>()->RegisterComponent(this);
	}

	RegisterHandlers();
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
		Handler->SetupHandle();
	}
	
	RunHandlers();
	
	GetOwner()->SetActorTickEnabled(false);
	Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(false);
}

void UBendTime_Register::OnEndTimeStop(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
		return;
	
	GetOwner()->SetActorTickEnabled(bActorTickEnabled);
	Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(bSimulatePhysics);

	EndHandlers();
}

void UBendTime_Register::RunHandlers()
{
	for (UActorComponent* Comp : GetOwner()->GetComponents())
	{
		if (UBendTime_Register_Handler_Base* Handler = GetHandler(Comp))
		{
			Handler->StartHandle(Comp);
		}
	}
}

void UBendTime_Register::EndHandlers()
{
	for (UActorComponent* Comp : GetOwner()->GetComponents())
	{
		if (UBendTime_Register_Handler_Base* Handler = GetHandler(Comp))
		{
			Handler->EndHandle(Comp);
		}
	}
}

void UBendTime_Register::RegisterHandlers()
{
	HandlerPointers.Add(NewObject<UBendTime_Register_Handler_Meshes>());
}

UBendTime_Register_Handler_Base* UBendTime_Register::GetHandler(UActorComponent* ActorComp)
{
	if (HandlerOptions.bMeshHandler && ActorComp->IsA(UMeshComponent::StaticClass()))
		return HandlerPointers[0];
	
	return nullptr;
}
