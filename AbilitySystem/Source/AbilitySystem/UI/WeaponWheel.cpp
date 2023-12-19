// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/_Misc/Helpers.h>
#include <Components/CanvasPanelSlot.h>
#include "WeaponWheelItem.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Fonts/SlateFontInfo.h"

void UWeaponWheel::NativeConstruct()
{
	Super::NativeConstruct();

	InitialWorldTimeDilation = GetWorld()->GetWorldSettings()->TimeDilation;

	SetupPlayer();
	SetupBoundWidgets();
	SpawnIconWidgets();
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

	if (ArrowImage)
	{
		UpdateArrow();
	}
}

void UWeaponWheel::SetupPlayer()
{
	PlayerPtr = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (IsValid(PlayerPtr))
	{
		if (APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
		{
			PC->SetShowMouseCursor(true);
			PC->SetIgnoreLookInput(true);
			PC->SetInputMode(FInputModeGameAndUI());
			
			GEngine->GameViewport->GetViewportSize(HalfScreenSize);
			HalfScreenSize *= 0.5f;
		
			PC->SetMouseLocation(HalfScreenSize.X, HalfScreenSize.Y * (1.f - RadiusAsPercent));
		}
	}
}

void UWeaponWheel::SetupBoundWidgets()
{
	if (IsValid(WheelParent))
	{
		if (UCanvasPanelSlot* WheelParentSlot = Cast<UCanvasPanelSlot>(WheelParent->Slot))
		{
			WheelParentSlot->SetSize(FVector2D(HalfScreenSize.GetMin() * RadiusAsPercent * 2.f));
			WheelParentSlot->SetAnchors(FAnchors(0.5f));
			WheelParentSlot->SetAlignment(FVector2D(0.5f));
		}
	}

	if (IsValid(ArrowImage))
	{
		if (UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
		{
			ArrowImageSlot->SetAnchors(FAnchors(0.5f));
			ArrowImageSlot->SetAlignment(FVector2D(0.5f));
			
			UpdateArrow();
		}
	}

	if (IsValid(SelectedItemImage))
	{
		if (UCanvasPanelSlot* SelectedItemImageSlot = Cast<UCanvasPanelSlot>(SelectedItemImage->Slot))
		{
			SelectedItemImageSlot->SetAnchors(FAnchors(0.5f, 0));
			SelectedItemImageSlot->SetAlignment(FVector2D(0.5f, 0));
			SelectedItemImageSlot->SetPosition(FVector2D(0, SI_Img_DistFromTop));
			SelectedItemImageSlot->SetSize(SI_Img_Size);
		}
	}

	if (IsValid(SelectedItemName))
	{
		if (UCanvasPanelSlot* SelectedItemNameSlot = Cast<UCanvasPanelSlot>(SelectedItemName->Slot))
		{
			SelectedItemNameSlot->SetAnchors(FAnchors(0.5f, 0));
			SelectedItemNameSlot->SetAlignment(FVector2D(0.5f, 0));
			SelectedItemNameSlot->SetPosition(FVector2D(0, SI_Name_DistFromTop));
		}
	}

	if (IsValid(SelectedItemDescription))
	{
		if (UCanvasPanelSlot* SelectedItemDescriptionSlot = Cast<UCanvasPanelSlot>(SelectedItemDescription->Slot))
		{
			SelectedItemDescriptionSlot->SetAnchors(FAnchors(0.5f, 0));
			SelectedItemDescriptionSlot->SetAlignment(FVector2D(0.5f, 0));
			SelectedItemDescriptionSlot->SetPosition(FVector2D(0, SI_Desc_DistFromTop));
		}
	}
}

void UWeaponWheel::SpawnIconWidgets()
{
	FVector2D WidgetLocation = FVector2D(0, -HalfScreenSize.GetMin() * RadiusAsPercent);
	
	for (int i = 0; i < WheelItems.Num(); i++)
	{
		if (!IsValid(WheelItems[i]))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Item at index %i not valid in UWeaponWheel::SpawnIconWidgets()"), i));
			continue;
		}
		
		WheelItemPtrs.Add(NewObject<UWeaponWheelItem>(this, WheelItems[i]));
		WheelItemPtrs.Last()->InitializeItem(WidgetTree, WheelParent, WidgetLocation, NormalItemSize, SelectedItemSize);
		
		WidgetLocation = WidgetLocation.GetRotated(360.f / WheelItems.Num());
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
			
			ArrowImageSlot->SetPosition(DirToMouse * HalfScreenSize.GetMin() * ArrowPositionAsPercent);
			ArrowImage->SetRenderTransformAngle(FVector(DirToMouse, 0).Rotation().Yaw);
			
			float SmallestAngle = 360.f;
				
			for (int i = 0; i < WheelItemPtrs.Num(); i++)
			{
				const float AngleDiff = FMath::Abs(FVector(DirToMouse, 0).Rotation().Yaw - WheelItemPtrs[i]->GetAngleFromCenter());
				
				if (AngleDiff <= 120.f && AngleDiff <= SmallestAngle)
				{
					SmallestAngle = AngleDiff;
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
