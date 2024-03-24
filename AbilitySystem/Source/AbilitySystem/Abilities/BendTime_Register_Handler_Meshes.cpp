// Fill out your copyright notice in the Description page of Project Settings.


#include "BendTime_Register_Handler_Meshes.h"

void UBendTime_Register_Handler_Meshes::StartHandle(AActor* ActorToHandle)
{
	Super::StartHandle(ActorToHandle);
	
	TArray<UMeshComponent*> Meshes;
	ActorToHandle->GetComponents<UMeshComponent>(Meshes);

	Velocities.Empty();
	
	for (const auto MeshPtr : Meshes)
	{
		Velocities.Add(MeshPtr, MeshPtr->GetPhysicsLinearVelocity());
		MeshPtr->PutAllRigidBodiesToSleep();
	}
}

void UBendTime_Register_Handler_Meshes::EndHandle(AActor* ActorToHandle)
{
	Super::EndHandle(ActorToHandle);

	TArray<UMeshComponent*> Meshes;
	ActorToHandle->GetComponents<UMeshComponent>(Meshes);
	
	for (const auto MeshPtr : Meshes)
	{
		const FVector* Vel = Velocities.Find(MeshPtr);
		MeshPtr->SetAllPhysicsLinearVelocity(Vel ? *Vel : FVector::ZeroVector);
		MeshPtr->WakeAllRigidBodies();
	}
}
