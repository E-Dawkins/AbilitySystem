// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "I_Door.generated.h"

/**
 * 
 */
UCLASS(HideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking))
class ABILITYSYSTEM_API AI_Door : public ABaseInteractable
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual bool StartInteract() override;

	void AnimateDoor();

private:
	// Can the door be toggled between open and closed?
	// Or can it only be opened once?
	UPROPERTY(EditAnywhere, Category = "Door")
	bool bToggleable = true;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	FVector OpenAnglePerAxis = FVector(0, 0, 90);

	UPROPERTY(EditAnywhere, Category = "Door", meta=(ClampMin = "0.001"))
	float DoorOpenSpeed = 0.001f;
	
private:
	bool bDoorOpen = false;
	bool bDoorAnimating = false;

	FRotator StartRotation;
	FRotator EndRotation;

	FRotator TargetRotation;
	
};
