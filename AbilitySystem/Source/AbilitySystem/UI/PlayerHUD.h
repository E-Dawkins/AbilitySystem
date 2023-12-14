// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetAbilityIcon();

protected:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;
	
};
