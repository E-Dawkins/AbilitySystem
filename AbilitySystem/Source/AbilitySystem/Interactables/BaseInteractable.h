// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.generated.h"


class APlayerCharacter;

UCLASS()
class ABILITYSYSTEM_API ABaseInteractable : public AActor
{
	GENERATED_BODY()
	
public:
	ABaseInteractable();
	
	virtual bool OnHover();
	virtual bool OnUnHover();
	virtual bool StartInteract(APlayerCharacter* _PlayerPtr);
	virtual bool EndInteract();

public:
	bool GetHoveredState() const { return bHovered; }
	bool GetInteractionState() const { return bInteractedWith; }

protected:
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* RootSceneComponent;
	
protected:
	bool bHovered = false;
	bool bInteractedWith = false;

	bool bCanInteract = true;

	UPROPERTY()
	APlayerCharacter* PlayerPtr;
};
