// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register_Handler_Meshes.h"

void UBendTime_Register_Handler_Meshes::SetupHandle()
{
	Super::SetupHandle();

	StoredData.Empty();
}

void UBendTime_Register_Handler_Meshes::StartHandle(UActorComponent* CompToHandle)
{
	Super::StartHandle(CompToHandle);

	if (UMeshComponent* MeshPtr = Cast<UMeshComponent>(CompToHandle))
	{
		StoredData.Add
		(
			MeshPtr,
			{MeshPtr->GetPhysicsLinearVelocity(), MeshPtr->GetPhysicsAngularVelocityInDegrees()}
		);
		
		MeshPtr->PutAllRigidBodiesToSleep();
	}
}

void UBendTime_Register_Handler_Meshes::EndHandle(UActorComponent* CompToHandle)
{
	Super::EndHandle(CompToHandle);

	if (UMeshComponent* MeshPtr = Cast<UMeshComponent>(CompToHandle))
	{
		if (const FMeshData* Velocities = StoredData.Find(MeshPtr))
		{
			MeshPtr->SetAllPhysicsLinearVelocity(Velocities->LinearVelocity);
			MeshPtr->SetAllPhysicsAngularVelocityInDegrees(Velocities->AngularVelocity);
		}
		
		MeshPtr->WakeAllRigidBodies();
	}
}
