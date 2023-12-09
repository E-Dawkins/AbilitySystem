// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWheel.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS(Abstract)
class ABILITYSYSTEM_API UWeaponWheel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;
	
};
