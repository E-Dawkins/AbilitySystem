// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "I_Throwable.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API AI_Throwable : public ABaseInteractable
{
	GENERATED_BODY()

public:
	virtual bool StartInteract(APlayerCharacter* _PlayerPtr) override;

	virtual void Tick(float DeltaSeconds) override;
	
	void Pickup();
	void Throw();

protected:
	// How far in front of the player to 'hold' the object
	UPROPERTY(EditAnywhere, Category = "Throwable")
	FVector HeldOffset = FVector(0, 50, 0);
	
protected:
	bool bHeld = false;
	
};
