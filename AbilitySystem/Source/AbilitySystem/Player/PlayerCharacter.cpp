// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "AbilitySystem/Interactables/BaseInteractable.h"
#include "AbilitySystem/UI/PlayerHUD.h"
#include "AbilitySystem/UI/WeaponWheel.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(PlayerHUDClass))
	{
		PlayerHUDPtr = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
		PlayerHUDPtr->AddToViewport();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentAbility && CurrentAbility.GetDefaultObject()->GetActiveState())
	{
		CurrentAbility.GetDefaultObject()->Update(DeltaTime);
	}

	CheckForInteractable();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Pressed, this, &APlayerCharacter::OnAbilityActivated);
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Released, this, &APlayerCharacter::OnAbilityUsed);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Pressed, this, &APlayerCharacter::OpenWeaponWheel);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Released, this, &APlayerCharacter::CloseWeaponWheel);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APlayerCharacter::StartInteract);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Released, this, &APlayerCharacter::EndInteract);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->OnDeactivation();
	}
}

void APlayerCharacter::OnAbilityActivated()
{
	// Don't activate the ability if the weapon wheel is open
	if (IsValid(WeaponWheelPtr) && WeaponWheelPtr->IsInViewport())
	{
		return;
	}
	
	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->OnActivation(this);
	}
}

void APlayerCharacter::OnAbilityUsed()
{
	if (CurrentAbility && CurrentAbility.GetDefaultObject()->GetActiveState())
	{
		CurrentAbility.GetDefaultObject()->OnUse();
	}
}

void APlayerCharacter::OpenWeaponWheel()
{
	if (!IsValid(WeaponWheelClass))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString("WeaponWheelClass Not Set!"));
		return;
	}
	
	if (!IsValid(WeaponWheelPtr) || !WeaponWheelPtr->IsInViewport())
	{
		WeaponWheelPtr = CreateWidget<UWeaponWheel>(GetWorld(), WeaponWheelClass);
		WeaponWheelPtr->AddToViewport();
	}

	// De-activate the currently activated ability, i.e. to stop player teleporting while wheel is open
	if (CurrentAbility && CurrentAbility.GetDefaultObject()->GetActiveState())
	{
		CurrentAbility.GetDefaultObject()->OnDeactivation();
	}
}

void APlayerCharacter::CloseWeaponWheel()
{
	if (IsValid(WeaponWheelPtr))
	{
		WeaponWheelPtr->RemoveFromViewport();
	}
}

void APlayerCharacter::StartInteract()
{
	if (IsValid(CurrentInteractable))
	{
		CurrentInteractable->StartInteract(this);
	}
}

void APlayerCharacter::EndInteract()
{
	if (IsValid(CurrentInteractable))
	{
		CurrentInteractable->EndInteract();
	}
}

void APlayerCharacter::CheckForInteractable()
{
	FHitResult Hit;

	FVector TraceStart;
	FRotator TraceDirection;
	GetActorEyesViewPoint(TraceStart, TraceDirection);

	const bool bTraceHit = GetWorld()->LineTraceSingleByChannel
	(
		Hit,
		TraceStart,
		TraceStart + TraceDirection.Vector() * InteractionRange,
		ECC_Visibility
	);

	if (bTraceHit)
	{
		ABaseInteractable* HitInteractable = Cast<ABaseInteractable>(Hit.GetActor());
		
		if (HitInteractable != CurrentInteractable)
		{
			if (IsValid(CurrentInteractable))
			{
				CurrentInteractable->OnUnHover();
			}
			
			CurrentInteractable = HitInteractable;

			if (IsValid(CurrentInteractable))
			{
				CurrentInteractable->OnHover();
			}
		}
	}
	else // nothing hit, nullify the current interactable
	{
		CurrentInteractable = nullptr;
	}
}

void APlayerCharacter::SetCurrentAbility(TSubclassOf<UBaseAbility> _NewAbility)
{
	CurrentAbility = _NewAbility;

	if (IsValid(PlayerHUDPtr))
	{
		PlayerHUDPtr->SetAbilityIcon();
	}
}
