// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInteractable.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBaseInteractable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseInteractable();

public:
	virtual bool OnHover();
	virtual bool OnUnHover();
	virtual bool StartInteract();
	virtual bool EndInteract();

public:
	bool GetHoveredState() const { return bHovered; }
	bool GetInteractionState() const { return bInteractedWith; }

protected:
	bool bHovered = false;
	bool bInteractedWith = false;

	bool bCanInteract = true;
		
};
