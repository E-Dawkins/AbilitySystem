// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/_Misc/Helpers.h>
#include <Components/CanvasPanelSlot.h>
#include "WeaponWheelItem.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Components/PanelWidget.h"

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
	if (WheelParent)
	{
		if (UCanvasPanelSlot* WheelParentSlot = Cast<UCanvasPanelSlot>(WheelParent->Slot))
		{
			WheelParentSlot->SetSize(FVector2D(HalfScreenSize.GetMin() * RadiusAsPercent * 2.f));
			WheelParentSlot->SetAnchors(FAnchors(0.5f));
			WheelParentSlot->SetAlignment(FVector2D(0.5f));
		}
	}

	if (ArrowImage)
	{
		if (UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
		{
			ArrowImageSlot->SetAnchors(FAnchors(0.5f));
			ArrowImageSlot->SetAlignment(FVector2D(0.5f));
			ArrowImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			
			UpdateArrow();
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
		WheelItemPtrs.Last()->InitializeItem(WidgetTree, WheelParent, WidgetLocation);
		
		WidgetLocation = WidgetLocation.GetRotated(360.f / WheelItems.Num());
	}
}

void UWeaponWheel::UpdateArrow()
{
	if (UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
	{
		if (const APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
		{
			FVector2D MousePos;
			const bool bIsMouseDevice = PC->GetMousePosition(MousePos.X, MousePos.Y);
			MousePos = !bIsMouseDevice ? HalfScreenSize : MousePos;

			FVector2D DirToMouse = (MousePos - HalfScreenSize).GetSafeNormal();
			DirToMouse = DirToMouse.Equals(FVector2D::ZeroVector) ? FVector2D(0, -1) : DirToMouse;
			
			ArrowImageSlot->SetPosition(DirToMouse * HalfScreenSize.GetMin() * ArrowPositionAsPercent);
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

			// Highlight the correct wheel item
			if (WheelItemPtrs.IsValidIndex(SelectedItemIndex))
			{
				if (LastSelectedItemIndex != SelectedItemIndex && WheelItemPtrs.IsValidIndex(LastSelectedItemIndex))
				{
					WheelItemPtrs[LastSelectedItemIndex]->ItemUnHover();
				}
					
				WheelItemPtrs[SelectedItemIndex]->ItemHover();
				LastSelectedItemIndex = SelectedItemIndex;
			}
		}
	}
}
