// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheelItem.h"

#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UWeaponWheelItem::InitializeItem(UWidgetTree* _WidgetTree, UPanelWidget* _ItemParent, FVector2D _Position, FVector2D _Size)
{
	if (!IsValid(_WidgetTree))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("_WidgetTree argument is null in UWeaponWheelItem::InitializeItem()"));
		return;
	}

	if (!IsValid(_ItemParent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("_ItemParent argument is null in UWeaponWheelItem::InitializeItem()"));
		return;
	}

	ImageWidget = _WidgetTree->ConstructWidget<UImage>();
	ImageWidget->SetBrushFromTexture(NormalIcon);

	_ItemParent->AddChild(ImageWidget);

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImageWidget->Slot))
	{
		CanvasSlot->SetAnchors(FAnchors(0.5f));
		CanvasSlot->SetPosition(_Position);

		FVector2D Alignment = _Position.GetSafeNormal();

		Alignment.X = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.X);
		Alignment.Y = FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0, 1), Alignment.Y);

		CanvasSlot->SetAlignment(Alignment);
		CanvasSlot->SetSize(_Size);
	}
}

void UWeaponWheelItem::ItemSelect()
{
	if (IsValid(ImageWidget) && IsValid(SelectedIcon))
	{
		
		ImageWidget->SetBrushFromTexture(SelectedIcon);
	}
}

void UWeaponWheelItem::ItemDeselect()
{
	if (IsValid(ImageWidget) && IsValid(NormalIcon))
	{
		ImageWidget->SetBrushFromTexture(NormalIcon);
	}
}

FVector2D UWeaponWheelItem::GetPosition() const
{
	if (IsValid(ImageWidget))
	{
		if (const UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImageWidget->Slot))
		{
			return CanvasSlot->GetPosition();
		}
	}

	return FVector2D::ZeroVector;
}

APlayerCharacter* UWeaponWheelItem::GetPlayerPtr()
{
	if (!IsValid(PlayerPtr))
	{
		PlayerPtr = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	}

	return PlayerPtr;
}
