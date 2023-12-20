// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeleportAbility.h"
#include "Ab_Blink.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_Blink : public UTeleportAbility
{
	GENERATED_BODY()

public:
	virtual void OnUse() override;
};
