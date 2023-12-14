// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Abilities/BaseAbility.h"
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

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
		MoveComp->MaxWalkSpeedCrouched = MaxCrouchSpeed;
	}

	if (CurrentAbility && CurrentAbility.GetDefaultObject()->GetActiveState())
	{
		CurrentAbility.GetDefaultObject()->Update(DeltaTime);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Actions
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::ToggleSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::ToggleSprint);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &APlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Pressed, this, &APlayerCharacter::OnAbilityActivated);
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Released, this, &APlayerCharacter::OnAbilityUsed);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Pressed, this, &APlayerCharacter::OpenWeaponWheel);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Released, this, &APlayerCharacter::CloseWeaponWheel);

	// Axis
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ACharacter::AddControllerYawInput);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->OnDeactivation();
	}
}

void APlayerCharacter::MoveForward(float _Value)
{
	AddMovementInput(GetActorForwardVector(), _Value);
}

void APlayerCharacter::MoveRight(float _Value)
{
	AddMovementInput(GetActorRightVector(), _Value);
}

void APlayerCharacter::ToggleCrouch()
{
	if (const UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		if (MoveComp->IsCrouching())
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}	
	}
}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
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
