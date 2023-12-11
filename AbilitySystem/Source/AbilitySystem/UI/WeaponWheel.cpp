// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include <AbilitySystem/Abilities/BaseAbility.h>
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

void UWeaponWheel::SpawnChildWidgets()
{
	for (const TSubclassOf<UUserWidget> Widget : AbilityWidgets)
	{
		UUserWidget* CreatedWidget = CreateWidget(this, Widget);
	
		if (UPanelWidget* Panel = Cast<UPanelWidget>(GetRootWidget()))
		{
			Panel->AddChild(CreatedWidget);
			
			FVector2D PlayerScreenSize = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(GetWorld()->GetFirstPlayerController()).GetLocalSize();
			Cast<UCanvasPanelSlot>(CreatedWidget->Slot)->SetPosition(PlayerScreenSize * 0.5f);
			Cast<UCanvasPanelSlot>(CreatedWidget->Slot)->SetAnchors(FAnchors());
			Cast<UCanvasPanelSlot>(CreatedWidget->Slot)->SetAlignment(FVector2D(0.5f));
		}
	}
}
