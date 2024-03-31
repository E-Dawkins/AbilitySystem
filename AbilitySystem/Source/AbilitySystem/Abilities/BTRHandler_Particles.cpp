// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRHandler_Particles.h"

#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"

void UBTRHandler_Particles::StartHandle(UActorComponent* CompToHandle)
{
	Super::StartHandle(CompToHandle);
	
	if (UParticleSystemComponent* ParticleComp = Cast<UParticleSystemComponent>(CompToHandle))
	{
		ParticleComp->CustomTimeDilation = 0;
	}
	else if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(CompToHandle))
	{
		NiagaraComp->SetCustomTimeDilation(0);
	}
}

void UBTRHandler_Particles::EndHandle(UActorComponent* CompToHandle)
{
	Super::EndHandle(CompToHandle);

	if (UParticleSystemComponent* ParticleComp = Cast<UParticleSystemComponent>(CompToHandle))
	{
		ParticleComp->CustomTimeDilation = 1;
	}
	else if (UNiagaraComponent* NiagaraComp = Cast<UNiagaraComponent>(CompToHandle))
	{
		NiagaraComp->SetCustomTimeDilation(1);
	}
}
