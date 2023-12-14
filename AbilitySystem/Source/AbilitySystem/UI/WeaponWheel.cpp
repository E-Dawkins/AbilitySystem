// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/_Misc/Helpers.h>
#include <Components/CanvasPanelSlot.h>

#include "PlayerHUD.h"
#include "AbilitySystem/Abilities/BaseAbility.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

void UWeaponWheel::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPtr = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

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
		PC->SetInputMode(FInputModeGameAndUI());

		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		HalfScreenSize = ViewportSize * 0.5f;
		
		PC->SetMouseLocation(HalfScreenSize.X, HalfScreenSize.Y - HalfScreenSize.Y * RadiusAsPercent);
    }

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

	if (Abilities.IsValidIndex(SelectedAbilityIndex))
	{
		PlayerPtr->SetCurrentAbility(Abilities[SelectedAbilityIndex]);

		if (UPlayerHUD* PlayerHUD = PlayerPtr->GetPlayerHUD())
		{
			PlayerHUD->SetAbilityIcon();
		}
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
	
	for (const TSubclassOf<UBaseAbility> Ability : Abilities)
	{
		if (WheelParent)
		{
			UImage* CreatedIcon = WidgetTree->ConstructWidget<UImage>();
			CreatedIcon->SetBrushFromTexture(Ability.GetDefaultObject()->NormalIcon);
			Icons.Add(CreatedIcon);
			
			WheelParent->AddChild(CreatedIcon);

			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CreatedIcon->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f));
				CanvasSlot->SetPosition(WidgetLocation);
				
				FVector2D Alignment = WidgetLocation.GetSafeNormal();

				Alignment.X = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.X);
				Alignment.Y = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.Y);

				CanvasSlot->SetAlignment(Alignment);
				CanvasSlot->SetSize(IconSize);
				
				WidgetLocation = WidgetLocation.GetRotated(360.f / static_cast<float>(Abilities.Num()));
			}
		}
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

			const float DegreesPerIcon = 360.f / static_cast<float>(Abilities.Num());

			float SmallestAngle = 360.f;
			static int LastAbilityIndex = 0;
				
			for (int i = 0; i < Icons.Num(); i++)
			{
				const FVector2D DirToIcon = Cast<UCanvasPanelSlot>(Icons[i]->Slot)->GetPosition().GetSafeNormal();

				const float Dot = FVector2D::DotProduct(DirToMouse, DirToIcon);
				const float ACosD = FMath::RadiansToDegrees(FMath::Acos(Dot));

				if (ACosD <= SmallestAngle && ACosD <= DegreesPerIcon)
				{
					SmallestAngle = ACosD;
					SelectedAbilityIndex = i;
				}
			}

			if (Abilities.IsValidIndex(SelectedAbilityIndex))
			{
				if (LastAbilityIndex != SelectedAbilityIndex)
				{
					Icons[LastAbilityIndex]->SetBrushFromTexture(Abilities[LastAbilityIndex].GetDefaultObject()->NormalIcon);
				}
					
				Icons[SelectedAbilityIndex]->SetBrushFromTexture(Abilities[SelectedAbilityIndex].GetDefaultObject()->HighlightedIcon);
				LastAbilityIndex = SelectedAbilityIndex;
			}
		}
	}
}
