// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRHandler_Meshes.h"

void UBTRHandler_Meshes::SetupHandle()
{
	Super::SetupHandle();

	StoredData.Empty();
}

void UBTRHandler_Meshes::StartHandle(UActorComponent* CompToHandle)
{
	Super::StartHandle(CompToHandle);

	if (UMeshComponent* MeshPtr = Cast<UMeshComponent>(CompToHandle))
	{
		const FMeshData Data =
		{
			MeshPtr->GetPhysicsLinearVelocity(),
			MeshPtr->GetPhysicsAngularVelocityInDegrees()
		};
		
		StoredData.Add(MeshPtr, Data);
		
		MeshPtr->PutAllRigidBodiesToSleep();
	}
}

void UBTRHandler_Meshes::EndHandle(UActorComponent* CompToHandle)
{
	Super::EndHandle(CompToHandle);

	if (UMeshComponent* MeshPtr = Cast<UMeshComponent>(CompToHandle))
	{
		if (const FMeshData* MeshData = StoredData.Find(MeshPtr))
		{
			MeshPtr->SetAllPhysicsLinearVelocity(MeshData->LinearVelocity);
			MeshPtr->SetAllPhysicsAngularVelocityInDegrees(MeshData->AngularVelocity);
		}
		
		MeshPtr->WakeAllRigidBodies();
	}
}
