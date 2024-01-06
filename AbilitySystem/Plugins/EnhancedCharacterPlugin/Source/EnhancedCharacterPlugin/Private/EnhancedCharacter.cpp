// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AEnhancedCharacter::AEnhancedCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CameraComponent->SetUsingAbsoluteLocation(true);
	CameraComponent->bUsePawnControlRotation = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AEnhancedCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initial cam location
	CamSmoothing_Offset = GetActorUpVector() * BaseEyeHeight;
	CameraComponent->SetWorldLocation(GetActorLocation() + CamSmoothing_Offset);

	// Initial 'sliding' variables
	InitialGroundFriction = GetCharacterMovement()->GroundFriction;
	InitialBrakingDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;
}

void AEnhancedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update speed based on crouch / sprint state
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bIsSprinting ? MaxSprintSpeed : MaxWalkSpeed;
		MoveComp->MaxWalkSpeedCrouched = MaxCrouchSpeed;
	}

	// Update camera location
	const FVector SmoothingTarget = GetActorLocation() + CamSmoothing_Offset;
	const float CurrentZ = UKismetMathLibrary::FInterpTo(CameraComponent->GetComponentLocation().Z, SmoothingTarget.Z, DeltaTime, CamSmoothing_Current);
	CameraComponent->SetWorldLocation(FVector(SmoothingTarget.X, SmoothingTarget.Y, CurrentZ));

	if (CameraComponent->GetComponentLocation().Z == SmoothingTarget.Z || GetCharacterMovement()->IsFalling())
	{
		CamSmoothing_Current = 0.f;
	}
}

void AEnhancedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(*MoveForwardMapping, this, &AEnhancedCharacter::MoveForward);
	PlayerInputComponent->BindAxis(*MoveRightMapping, this, &AEnhancedCharacter::MoveRight);
	PlayerInputComponent->BindAxis(*LookUpMapping, this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(*LookRightMapping, this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(*JumpMapping, IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction<FInputEventDelegate>(*SprintMapping, IE_Pressed, this, &AEnhancedCharacter::ToggleSprint, IE_Pressed);
	PlayerInputComponent->BindAction<FInputEventDelegate>(*SprintMapping, IE_Released, this, &AEnhancedCharacter::ToggleSprint, IE_Released);
	PlayerInputComponent->BindAction<FInputEventDelegate>(*CrouchMapping, IE_Pressed, this, &AEnhancedCharacter::ToggleCrouch, IE_Pressed);
	PlayerInputComponent->BindAction<FInputEventDelegate>(*CrouchMapping, IE_Released, this, &AEnhancedCharacter::ToggleCrouch, IE_Released);
}

void AEnhancedCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	CamSmoothing_Current = CameraSmoothing;
	CamSmoothing_Offset = GetActorUpVector() * CrouchedEyeHeight;
}

void AEnhancedCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	CamSmoothing_Current = CameraSmoothing;
	CamSmoothing_Offset = GetActorUpVector() * BaseEyeHeight;
}

void AEnhancedCharacter::MoveForward(float _Value)
{
	AddMovementInput(GetActorForwardVector(), _Value);
}

void AEnhancedCharacter::MoveRight(float _Value)
{
	AddMovementInput(GetActorRightVector(), _Value);
}

void AEnhancedCharacter::ToggleSprint(EInputEvent _InputEvent)
{
	if (bToggleSprint)
	{
		bIsSprinting = (_InputEvent == IE_Pressed) ? !bIsSprinting : bIsSprinting;
	}
	else
	{
		bIsSprinting = (_InputEvent == IE_Pressed);
	}

	if (bIsSprinting)
	{
		if (bIsCrouched)
		{
			UnCrouch();
			bShouldCrouch = false;
		}

		if (bIsSliding)
		{
			ToggleSlide();
		}
	}
}

void AEnhancedCharacter::ToggleCrouch(EInputEvent _InputEvent)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// Determine crouched state
	if (bToggleCrouch)
	{
		if (_InputEvent == IE_Pressed)
		{
			bShouldCrouch = !bIsCrouched;
		}
	}
	else
	{
		if (_InputEvent == IE_Pressed && !bIsCrouched)
		{
			bShouldCrouch = true;
		}
		else if (_InputEvent == IE_Released && bIsCrouched)
		{
			bShouldCrouch = false;
		}
	}

	// Update crouched + sliding state
	if (bShouldCrouch)
	{
		Crouch();

		if (bIsSprinting && !bIsSliding)
		{
			ToggleSlide();
		}
	}
	else
	{
		UnCrouch();

		if (bIsSliding)
		{
			ToggleSlide();
		}
	}
}

void AEnhancedCharacter::ToggleSlide()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		// Start slide
		if (!bIsSliding && bCanSlide && GetVelocity().Size() >= ((MaxWalkSpeed + MaxSprintSpeed) * 0.5f))
		{
			MoveComp->GroundFriction = 0.f;
			MoveComp->BrakingDecelerationWalking = 0.f;
			
			Controller->SetIgnoreMoveInput(true);

			GetWorld()->GetTimerManager().SetTimer(SlidingHandle, [this] {ToggleSlide();}, SlideTime, false);
		}

		// End slide
		else
		{
			MoveComp->GroundFriction = InitialGroundFriction;
			MoveComp->BrakingDecelerationWalking = InitialBrakingDeceleration;

			Controller->SetIgnoreMoveInput(false);
			
			GetWorld()->GetTimerManager().ClearTimer(SlidingHandle);
		}
	}

	bIsSliding = Controller->IsMoveInputIgnored();
}
