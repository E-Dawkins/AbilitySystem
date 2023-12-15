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
	virtual void InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position);
	virtual void ItemHover();
	virtual void ItemUnHover();
	virtual void ItemSelect();

	FVector2D GetPosition() const;
	APlayerCharacter* GetPlayerPtr();

private:
	UPROPERTY(EditAnywhere)
	UTexture2D* NormalIcon;

	UPROPERTY(EditAnywhere)
	UTexture2D* SelectedIcon;

	UPROPERTY(EditAnywhere)
	FVector2D NormalSize = FVector2D(125);

	UPROPERTY(EditAnywhere)
	FVector2D SelectedSize = FVector2D(140);
	
private:
	UPROPERTY()
	UImage* ImageWidget;

	UPROPERTY()
	APlayerCharacter* PlayerPtr;
	
};
