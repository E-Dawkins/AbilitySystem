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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void SpawnChildWidgets();

protected:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerPtr;

	// The time dilation while the weapon wheel is open
	UPROPERTY(EditAnywhere, meta=(ClampMin = "0.1", ClampMax = "1.0"))
	float WheelTimeDilation = 0.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UUserWidget>> AbilityWidgets;
	
	// The percent of the smallest screen length, that the wheel should cover
	// i.e. Screen = (100, 200) | Percent = 0.25 | Radius = 100 * 0.25 = 25 pixels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = "0.0", ClampMax = "0.9"))
	float RadiusAsPercent = 0.65f;

	UPROPERTY(EditAnywhere)
	FVector2D IconSize = FVector2D(50, 50);
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UPanelWidget* WheelParent;
	
protected:
	float InitialWorldTimeDilation = 1.f;
	float CurrentWorldTimeDilation = 1.f;
	
};
