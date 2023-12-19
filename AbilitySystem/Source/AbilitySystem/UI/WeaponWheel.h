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
	void SetupPlayer();
	void SetupBoundWidgets();
	void SpawnIconWidgets();
	void UpdateArrow();
	void UpdateSelectedItem();

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;
	
protected:
	// The time dilation while the weapon wheel is open
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.1", ClampMax = "1.0"), Category = "Wheel")
	float WheelTimeDilation = 0.25f;
	
	// The percent of the smallest screen length, that the wheel should cover
	// i.e. Screen = (100, 200) | Percent = 0.25 | Radius = 100 * 0.25 = 25 pixels
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "0.9"), Category = "Wheel")
	float RadiusAsPercent = 0.9f;

	// Same logic as RadiusAsPercent, i.e percent of the smallest screen length
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "0.9"), Category = "Wheel")
	float ArrowPositionAsPercent = 0.625f;

	UPROPERTY(EditAnywhere, Category = "Wheel|Items")
	TArray<TSubclassOf<UWeaponWheelItem>> WheelItems;
	
	UPROPERTY(EditAnywhere, Category = "Wheel|Items")
	FVector2D NormalItemSize = FVector2D(125);

	UPROPERTY(EditAnywhere, Category = "Wheel|Items")
	FVector2D SelectedItemSize = FVector2D(140);

	// Distance in slate units from the top of the wheel
	UPROPERTY(EditAnywhere, Category = "Wheel|Selected Item", DisplayName = "Image - From Top")
	float SI_Img_DistFromTop = 150.f;

	// Size in slate units
	UPROPERTY(EditAnywhere, Category = "Wheel|Selected Item", DisplayName = "Image - Size")
	FVector2D SI_Img_Size = FVector2D(225.f);

	// Distance in slate units from the top of the wheel
	UPROPERTY(EditAnywhere, Category = "Wheel|Selected Item", DisplayName = "Name - From Top")
	float SI_Name_DistFromTop = 400.f;

	UPROPERTY(EditAnywhere, Category = "Wheel|Selected Item", DisplayName = "Description - From Top")
	float SI_Desc_DistFromTop = 500.f;
	
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
	
	FVector2D HalfScreenSize = FVector2D::UnitVector;
	
	float InitialWorldTimeDilation = 1.f;
	float CurrentWorldTimeDilation = 1.f;

	int LastSelectedItemIndex = -1;
	int SelectedItemIndex = -1;
	
};
