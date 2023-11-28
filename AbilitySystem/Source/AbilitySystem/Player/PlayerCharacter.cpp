// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Abilities/BaseAbility.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->OnActivation(GetWorld());
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (auto MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
		MoveComp->MaxWalkSpeedCrouched = MaxCrouchSpeed;
	}

	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->Update(this, DeltaTime);
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
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Pressed, this, &APlayerCharacter::OnAbilityUsed);

	// Axis
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ACharacter::AddControllerYawInput);
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
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::ToggleSprint()
{
	bIsSprinting = !bIsSprinting;
}

void APlayerCharacter::OnAbilityUsed()
{
	if (CurrentAbility)
	{
		CurrentAbility.GetDefaultObject()->OnUse(this);
	}
}
