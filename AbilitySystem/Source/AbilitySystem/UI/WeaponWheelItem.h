// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "WeaponWheelItem.generated.h"

class APlayerCharacter;
class UImage;
class UPanelWidget;
class UWidgetTree;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ABILITYSYSTEM_API UWeaponWheelItem : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position, FVector2D _Size);
	virtual void ItemSelect();
	virtual void ItemDeselect();

	FVector2D GetPosition() const;
	APlayerCharacter* GetPlayerPtr();

private:
	UPROPERTY(EditAnywhere)
	UTexture2D* NormalIcon;

	UPROPERTY(EditAnywhere)
	UTexture2D* SelectedIcon;
	
private:
	UPROPERTY()
	UImage* ImageWidget;

	UPROPERTY()
	APlayerCharacter* PlayerPtr;
	
};
