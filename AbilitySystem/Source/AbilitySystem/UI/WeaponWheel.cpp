// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/_Misc/Helpers.h>
#include <Components/CanvasPanelSlot.h>
#include "AbilitySystem/Player/PlayerCharacter.h"
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
		PC->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
		
		HalfScreenSize = ViewportSize * 0.5f;
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
		if (UCanvasPanelSlot* ArrowImageSlot = Cast<UCanvasPanelSlot>(ArrowImage->Slot))
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerPtr->GetController()))
			{
				FVector2D MousePos;
				PC->GetMousePosition(MousePos.X, MousePos.Y);

				const FVector2D DirToMouse = (MousePos - HalfScreenSize).GetSafeNormal();
				
				if (DirToMouse != FVector2D::ZeroVector)
				{
					ArrowImageSlot->SetPosition(DirToMouse * HalfScreenSize.GetMin() * ArrowPositionAsPercent);
					
					float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector2D::DotProduct(FVector2D(0, -1), DirToMouse)));

					if (FVector2D::CrossProduct(FVector2D(0, -1), DirToMouse) < 0)
					{
						AngleInDegrees = -AngleInDegrees;
					}
					
					ArrowImage->SetRenderTransformAngle(AngleInDegrees);
				}
			}
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
			ArrowImageSlot->SetPosition(FVector2D(0, -HalfScreenSize.GetMin() * ArrowPositionAsPercent));
			ArrowImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UWeaponWheel::SpawnIconWidgets()
{
	FVector2D WidgetLocation = FVector2D(0, -HalfScreenSize.GetMin() * RadiusAsPercent);
	
	for (const TSubclassOf<UUserWidget> Widget : AbilityWidgets)
	{
		UUserWidget* CreatedWidget = CreateWidget(this, Widget);
	
		if (WheelParent)
		{
			WheelParent->AddChild(CreatedWidget);

			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CreatedWidget->Slot))
			{
				CanvasSlot->SetAnchors(FAnchors(0.5f));
				CanvasSlot->SetPosition(WidgetLocation);

				FVector2D Alignment = WidgetLocation.GetSafeNormal();

				Alignment.X = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.X);
				Alignment.Y = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.Y);

				CanvasSlot->SetAlignment(Alignment);
				CanvasSlot->SetSize(IconSize);

				WidgetLocation = WidgetLocation.GetRotated(360.f / static_cast<float>(AbilityWidgets.Num()));
			}
		}
	}
}
