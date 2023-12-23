// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractable.h"

// Sets default values for this component's properties
UBaseInteractable::UBaseInteractable()
{
	
}

void UBaseInteractable::OnHover()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("On Hover"));
}

void UBaseInteractable::OnUnHover()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("On Un-Hover"));
}

void UBaseInteractable::StartInteract()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Purple, FString("Started Interaction"));
}
