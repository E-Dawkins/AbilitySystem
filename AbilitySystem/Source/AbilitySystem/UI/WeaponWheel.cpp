// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/_Misc/Helpers.h>
#include <Components/CanvasPanelSlot.h>
#include "WeaponWheelItem.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UWeaponWheel::NativeConstruct()
{
	Super::NativeConstruct();

	InitialWorldTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;

	GEngine->GameViewport->GetViewportSize(HalfScreenSize);
	HalfScreenSize *= 0.5f;

	PlayerPtr = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	
	SetupBoundWidgets();
	SpawnIconWidgets();
	SetupPlayer();
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
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (WheelItemPtrs.IsValidIndex(SelectedItemIndex))
	{
		WheelItemPtrs[SelectedItemIndex]->ItemSelect();
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

	if (IsValid(ArrowImage))
	{
		UpdateArrow();
	}
}

void UWeaponWheel::SetupPlayer() const
{
	if (IsValid(PlayerPtr))
	{
		if (APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
		{
			PC->SetShowMouseCursor(true);
			PC->SetIgnoreLookInput(true);
			PC->SetInputMode(FInputModeGameAndUI());
			PC->SetMouseLocation(HalfScreenSize.X, HalfScreenSize.Y - HalfWheelSize.Y);
		}
	}
}

void UWeaponWheel::SetupBoundWidgets()
{
	if (IsValid(WheelParent))
	{
		if (const UCanvasPanelSlot* WheelParentSlot = Cast<UCanvasPanelSlot>(WheelParent->Slot))
		{
			HalfWheelSize = WheelParentSlot->GetSize() * 0.5f;
		}
	}

	if (IsValid(ArrowImage))
	{
		if (const UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
		{
			ArrowStartPosition = ArrowImageSlot->GetPosition();
			UpdateArrow();
		}
	}
}

void UWeaponWheel::SpawnIconWidgets()
{
	FVector2D WidgetLocation = FVector2D(0, -HalfWheelSize.Y); // start from top-middle
	
	for (int i = 0; i < WheelItems.Num(); i++)
	{
		if (!IsValid(WheelItems[i]))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Item at index %i not valid in UWeaponWheel::SpawnIconWidgets()"), i));
			continue;
		}
		
		WheelItemPtrs.Add(NewObject<UWeaponWheelItem>(this, WheelItems[i]));
		WheelItemPtrs.Last()->InitializeItem(WidgetTree, WheelParent, WidgetLocation, NormalItemSize, SelectedItemSize);
		
		WidgetLocation = WidgetLocation.GetSafeNormal().GetRotated(360.f / WheelItems.Num()) * HalfWheelSize;
	}
}

void UWeaponWheel::UpdateArrow()
{
	if (UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
	{
		if (const APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
		{
			FVector2D DirToMouse;
			const bool bMouseDevice = PC->GetMousePosition(DirToMouse.X, DirToMouse.Y);
			DirToMouse = bMouseDevice ? (DirToMouse - HalfScreenSize).GetSafeNormal() : FVector2D(0, -1);
			
			ArrowImageSlot->SetPosition(DirToMouse * ArrowStartPosition.Size());
			ArrowImage->SetRenderTransformAngle(FVector(DirToMouse, 0).Rotation().Yaw);
			
			float SmallestAngle = 360.f;
				
			for (int i = 0; i < WheelItemPtrs.Num(); i++)
			{
				const float Dot = FVector2D::DotProduct(DirToMouse, WheelItemPtrs[i]->GetPosition().GetSafeNormal());
				const float ACosD = FMath::RadiansToDegrees(FMath::Acos(Dot));

				if (ACosD <= SmallestAngle)
				{
					SmallestAngle = ACosD;
					SelectedItemIndex = i;
				}
			}

			UpdateSelectedItem();
		}
	}
}

void UWeaponWheel::UpdateSelectedItem()
{
	if (LastSelectedItemIndex == SelectedItemIndex || WheelItemPtrs.Num() == 0)
	{
		return;
	}
	
	if (WheelItemPtrs.IsValidIndex(LastSelectedItemIndex) && IsValid(WheelItemPtrs[LastSelectedItemIndex]))
	{
		WheelItemPtrs[LastSelectedItemIndex]->ItemUnHover();
	}

	if (WheelItemPtrs.IsValidIndex(SelectedItemIndex) && IsValid(WheelItemPtrs[SelectedItemIndex]))
	{
		WheelItemPtrs[SelectedItemIndex]->ItemHover();
		LastSelectedItemIndex = SelectedItemIndex;

		if (IsValid(SelectedItemImage))
		{
			SelectedItemImage->SetBrushFromTexture(WheelItemPtrs[SelectedItemIndex]->GetNormalIcon());
		}

		if (IsValid(SelectedItemName))
		{
			SelectedItemName->SetText(FText::FromString(WheelItemPtrs[SelectedItemIndex]->GetItemName()));
		}

		if (IsValid(SelectedItemDescription))
		{
			SelectedItemDescription->SetText(FText::FromString(WheelItemPtrs[SelectedItemIndex]->GetItemDescription()));
		}
	}
}
