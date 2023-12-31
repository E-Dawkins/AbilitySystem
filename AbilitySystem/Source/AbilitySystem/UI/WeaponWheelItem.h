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
	virtual void InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position, FVector2D _NormalSize, FVector2D _SelectedSize);
	virtual void ItemHover();
	virtual void ItemUnHover();
	virtual void ItemSelect();

	// -- Helpers --
	FVector2D GetPosition() const;
	APlayerCharacter* GetPlayerPtr();
	UTexture2D* GetNormalIcon() const;
	FString GetItemName() const;
	FString GetItemDescription() const;

protected:
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bHideIconsInInspector", EditConditionHides))
	UTexture2D* NormalIcon;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bHideIconsInInspector", EditConditionHides))
	UTexture2D* SelectedIcon;

	UPROPERTY(EditAnywhere)
	FString ItemName = "Item";
	
	UPROPERTY(EditAnywhere)
	FString ItemDescription = "This is a short description for the selected item.";
	
protected:
	UPROPERTY()
	bool bHideIconsInInspector = false;
	
	UPROPERTY()
	UImage* ImageWidget;

	UPROPERTY()
	APlayerCharacter* PlayerPtr;

	FVector2D NormalSize = FVector2D(100);
	FVector2D SelectedSize = FVector2D(150);
	
};
