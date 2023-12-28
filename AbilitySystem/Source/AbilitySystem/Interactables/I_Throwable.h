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
	
	void Pickup() const;
	void Throw(bool _bWithForce = true) const;

protected:
	// How far in front of the player to 'hold' the object
	UPROPERTY(EditAnywhere, Category = "Throwable")
	float HeldOffset = 150.f;

	UPROPERTY(EditAnywhere, Category = "Throwable")
	float ThrowForce = 5000.f;
	
protected:
	bool bHeld = false;
	
};
