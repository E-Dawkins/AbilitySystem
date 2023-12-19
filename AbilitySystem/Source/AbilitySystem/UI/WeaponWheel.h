// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWheel.generated.h"

class UTextBlock;
class UImage;
class UWeaponWheelItem;
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void SetupPlayer() const;
	void SetupBoundWidgets();
	void SpawnIconWidgets();
	void UpdateArrow();
	void UpdateSelectedItem();

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;
	
protected:
	// The time dilation while the weapon wheel is open
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.1", ClampMax = "1.0"))
	float WheelTimeDilation = 0.25f;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UWeaponWheelItem>> WheelItems;
	
	UPROPERTY(EditAnywhere)
	FVector2D NormalItemSize = FVector2D(125);

	UPROPERTY(EditAnywhere)
	FVector2D SelectedItemSize = FVector2D(140);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UPanelWidget* WheelParent;

	UPROPERTY(meta=(BindWidget))
	UImage* ArrowImage;

	UPROPERTY(meta=(BindWidget))
	UImage* SelectedItemImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SelectedItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SelectedItemDescription;

	TArray<UWeaponWheelItem*> WheelItemPtrs;
	
	FVector2D HalfWheelSize = FVector2D(0);
	FVector2D ArrowStartPosition = FVector2D(0);
	FVector2D HalfScreenSize = FVector2D::UnitVector;
	
	float InitialWorldTimeDilation = 1.f;
	float CurrentWorldTimeDilation = 1.f;

	int LastSelectedItemIndex = -1;
	int SelectedItemIndex = -1;
	
};
