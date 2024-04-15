// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register.h"
#include "BendTime_Manager.h"
#include "BTRHandler_AI.h"
#include "BTRHandler_Animation.h"
#include "BTRHandler_Base.h"
#include "BTRHandler_Meshes.h"
#include "BTRHandler_Particles.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UBendTime_Register::UBendTime_Register()
	: bIgnoreTimeBend(false)
	, HandlerOptions(FHandlerOptions())
	, HandlerPointers(TArray<UBTRHandler_Base*>())
	, CustomDilation(1.f)
	, bActorTickEnabled(true)
	, bSimulatePhysics(false)
{
	
}

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

void UBendTime_Register::OnEndTimeSlow(FTimeBendOptions Options) const
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

	if (const auto PrimComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
		PrimComp->SetSimulatePhysics(false);
}

void UBendTime_Register::OnEndTimeStop(FTimeBendOptions Options)
{
	if (bIgnoreTimeBend)
		return;
	
	GetOwner()->SetActorTickEnabled(bActorTickEnabled);

	if (const auto PrimComp = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
		PrimComp->SetSimulatePhysics(bSimulatePhysics);

	EndHandlers();
}

void UBendTime_Register::RunHandlers()
{
	// Special handlers, just run them with root component
	if (HandlerOptions.bAIHandler) HandlerPointers[2]->StartHandle(GetOwner()->GetRootComponent());
	
	// Other handlers are ran with their respective components
	for (UActorComponent* Comp : GetOwner()->GetComponents())
	{
		for (const auto Handler : GetHandlers(Comp))
		{
			Handler->StartHandle(Comp);
		}
	}
}

void UBendTime_Register::EndHandlers()
{
	// Special handlers, just run them with this component
	if (HandlerOptions.bAIHandler) HandlerPointers[2]->EndHandle(GetOwner()->GetRootComponent());
	
	// Other handlers are ran with their respective components
	for (UActorComponent* Comp : GetOwner()->GetComponents())
	{
		for (const auto Handler : GetHandlers(Comp))
		{
			Handler->EndHandle(Comp);
		}
	}
}

void UBendTime_Register::RegisterHandlers()
{
	HandlerPointers.Add(NewObject<UBTRHandler_Meshes>());
	HandlerPointers.Add(NewObject<UBTRHandler_Particles>());
	HandlerPointers.Add(NewObject<UBTRHandler_AI>());
	HandlerPointers.Add(NewObject<UBTRHandler_Animation>());
}

TArray<UBTRHandler_Base*> UBendTime_Register::GetHandlers(const UActorComponent* ActorComp)
{
	TArray<UBTRHandler_Base*> Handlers;
	
	if (HandlerOptions.bMeshHandler && ActorComp->IsA(UMeshComponent::StaticClass()))
		Handlers.Add(HandlerPointers[0]);

	if (HandlerOptions.bParticleHandler && ActorComp->IsA(UParticleSystemComponent::StaticClass()))
		Handlers.Add(HandlerPointers[1]);
	
	if (HandlerOptions.bParticleHandler && ActorComp->IsA(UNiagaraComponent::StaticClass()))
		Handlers.Add(HandlerPointers[1]);

	if (HandlerOptions.bAnimHandler && ActorComp->IsA(USkeletalMeshComponent::StaticClass()))
		Handlers.Add(HandlerPointers[3]);
	
	return Handlers;
}
