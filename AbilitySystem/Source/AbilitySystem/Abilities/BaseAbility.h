// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ABILITYSYSTEM_API UBaseAbility : public UObject
{
	GENERATED_BODY()

public:
	// Put set-up code here, i.e. spawning a cursor particle
	// This is the only virtual function that needs to be overriden to properly set-up ability
	virtual void OnActivation(APlayerCharacter* _Player);

	// Put actual ability code here, i.e. teleport the player
	virtual void OnUse();

	virtual void Update(float _DeltaSeconds);

	virtual void OnDeactivation();

public:
	bool GetActiveState() const { return bAbilityActive; }

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* NormalIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* HighlightedIcon;
	
protected:
	UPROPERTY()
	APlayerCharacter* PlayerPtr;
	bool bAbilityActive = false;
	
};
