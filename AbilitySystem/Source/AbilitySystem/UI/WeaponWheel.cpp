// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheel.h"

#include "AbilitySystem/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
