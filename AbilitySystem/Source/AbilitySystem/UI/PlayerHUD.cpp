// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "AbilitySystem/Player/PlayerCharacter.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPtr = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	
	SetAbilityIcon();
}
