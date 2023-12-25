// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "Components/TimelineComponent.h"
#include "I_Door.generated.h"

/**
 * 
 */
UCLASS(HideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class ABILITYSYSTEM_API AI_Door : public ABaseInteractable
{
	GENERATED_BODY()

public:
	AI_Door();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual bool StartInteract() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* DoorMesh;
	
	// Can the door be toggled between open and closed?
	// Or can it only be opened once?
	UPROPERTY(EditAnywhere, Category = "Door")
	bool bToggleable = true;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	FRotator AnglePerAxis = FRotator(0, 90, 0);

	UPROPERTY(EditAnywhere, Category = "Door", meta=(ClampMin = "0.1"))
	float DoorOpenTime = 0.3f;
	
private:
	bool bDoorOpen = false;

	FRotator StartRotation;
	FRotator EndRotation;

	FTimeline DoorTimeline;
	
};
