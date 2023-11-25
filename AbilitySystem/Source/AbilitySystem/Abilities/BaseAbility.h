// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Player/PlayerCharacter.h"
#include "BaseAbility.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ABILITYSYSTEM_API UBaseAbility : public UObject
{
	GENERATED_BODY()

public:
	// Put set-up code here, i.e. spawning a cursor particle
	virtual void OnActivation(UWorld* World);

	// Put actual ability code here, i.e. teleport the player
	virtual void OnUse(APlayerCharacter* Player);

	virtual void Update(APlayerCharacter* Player, float DeltaSeconds);

	virtual void OnDeactivation();
	
};
