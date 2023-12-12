// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/Abilities/BaseAbility.h>
#include <AbilitySystem/_Misc/Helpers.h>
#include <Blueprint/WidgetTree.h>
#include <Components/CanvasPanelSlot.h>

#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UWeaponWheel::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPtr = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	InitialWorldTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;
	
	if (!IsValid(PlayerPtr))
    {
    	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Player is null in WeaponWheel->NativeConstruct()"));
    	return;
    }
    
	if (APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
    {
    	PC->SetShowMouseCursor(true);
		PC->SetIgnoreLookInput(true);

		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		PC->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);

		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC);
    }

	SpawnChildWidgets();
}

void UWeaponWheel::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (WheelTimeDilation < InitialWorldTimeDilation)
	{
		FHelpers::SetGlobalDilation(GetWorld(), InitialWorldTimeDilation);
	}

	if (!IsValid(PlayerPtr))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Player is null in WeaponWheel->NativeDestruct()"));
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
	{
		PC->SetShowMouseCursor(false);
		PC->ResetIgnoreLookInput();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);
	}
}

void UWeaponWheel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentWorldTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;

	if (WheelTimeDilation < CurrentWorldTimeDilation)
	{
		FHelpers::SetGlobalDilation(GetWorld(), WheelTimeDilation);
		InitialWorldTimeDilation = CurrentWorldTimeDilation;
	}
}

void UWeaponWheel::SpawnChildWidgets()
{
	const FVector2D HalfScreenSize = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(GetWorld()->GetFirstPlayerController()).GetLocalSize() * 0.5f;
	const float Radius = HalfScreenSize.GetMin() * RadiusAsPercent;
	
	if (UCanvasPanelSlot* WheelParentSlot = Cast<UCanvasPanelSlot>(WheelParent->Slot))
	{
		WheelParentSlot->SetSize(FVector2D(Radius * 2.f));
		WheelParentSlot->SetAnchors(FAnchors(0.5f));
		WheelParentSlot->SetAlignment(FVector2D(0.5f));
	}
	
	FVector2D WidgetLocation = FVector2D(0, -Radius);
	
	for (const TSubclassOf<UUserWidget> Widget : AbilityWidgets)
	{
		UUserWidget* CreatedWidget = CreateWidget(this, Widget);
	
		if (UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget()))
		{
			Panel->AddChild(CreatedWidget);

			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CreatedWidget->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f));
				CanvasSlot->SetPosition(WidgetLocation);

				FVector2D Alignment = WidgetLocation.GetSafeNormal();

				Alignment.X = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.X);
				Alignment.Y = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.Y);

				CanvasSlot->SetAlignment(Alignment);
				CanvasSlot->SetSize(IconSize);

				WidgetLocation = WidgetLocation.GetRotated(360.f / (float)AbilityWidgets.Num());
			}
		}
	}
}
