// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWheel.generated.h"

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

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UWeaponWheelItem>> WheelItems;
	
	// The time dilation while the weapon wheel is open
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.1", ClampMax = "1.0"))
	float WheelTimeDilation = 0.25f;
	
	// The percent of the smallest screen length, that the wheel should cover
	// i.e. Screen = (100, 200) | Percent = 0.25 | Radius = 100 * 0.25 = 25 pixels
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "0.9"))
	float RadiusAsPercent = 0.65f;

	// Same logic as RadiusAsPercent, i.e percent of the smallest screen length
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "0.9"))
	float ArrowPositionAsPercent = 0.5f;

	UPROPERTY(meta=(BindWidget))
	UPanelWidget* WheelParent;

	UPROPERTY(meta=(BindWidget))
	UImage* ArrowImage;
	
protected:
	TArray<UWeaponWheelItem*> WheelItemPtrs;
	
	FVector2D HalfScreenSize = FVector2D::UnitVector;
	
	float InitialWorldTimeDilation = 1.f;
	float CurrentWorldTimeDilation = 1.f;

	int LastSelectedItemIndex = -1;
	int SelectedItemIndex = -1;
	
};
