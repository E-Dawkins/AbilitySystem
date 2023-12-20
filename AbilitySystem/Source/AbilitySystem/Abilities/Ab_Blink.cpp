// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Blink.h"

#include "AbilitySystem/Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_Blink::OnUse()
{
	Super::OnUse();

	if (!PlayerPtr)
	{
		return;
	}

	// Disable player input / collision temporarily
	PlayerPtr->DisableInput(PlayerPtr->GetController<APlayerController>());
	PlayerPtr->SetActorEnableCollision(false);
	
	if (bShouldCrouch)
	{
		PlayerPtr->Crouch();
	}
	
	PlayerPtr->SetActorLocation(TeleportLocation);
	PlayerPtr->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	OnDeactivation();
}
