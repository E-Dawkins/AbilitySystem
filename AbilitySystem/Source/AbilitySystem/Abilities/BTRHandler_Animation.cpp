// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRHandler_Animation.h"

void UBTRHandler_Animation::StartHandle(UActorComponent* CompToHandle)
{
	Super::StartHandle(CompToHandle);

	if (const auto SkeletalMesh = Cast<USkeletalMeshComponent>(CompToHandle))
	{
		SkeletalMesh->bPauseAnims = true;
	}
}

void UBTRHandler_Animation::EndHandle(UActorComponent* CompToHandle)
{
	Super::EndHandle(CompToHandle);

	if (const auto SkeletalMesh = Cast<USkeletalMeshComponent>(CompToHandle))
    {
		SkeletalMesh->bPauseAnims = false;
    }
}
