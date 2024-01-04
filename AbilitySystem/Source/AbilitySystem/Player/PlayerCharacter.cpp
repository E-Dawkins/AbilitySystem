// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Abilities/BaseAbility.h"
#include "AbilitySystem/Interactables/BaseInteractable.h"
#include "AbilitySystem/UI/PlayerHUD.h"
#include "AbilitySystem/UI/WeaponWheel.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CameraComponent->SetUsingAbsoluteLocation(true);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(PlayerHUDClass))
	{
		PlayerHUDPtr = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);
		PlayerHUDPtr->AddToViewport();
	}

	// Initial camera location
	CameraComponent->SetWorldLocation(GetCameraTargetLocation());
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

	CheckForInteractable();

	// Update camera location
	const FVector CamTargetLocation = GetCameraTargetLocation();
	const float CurrentZ = UKismetMathLibrary::FInterpTo(CameraComponent->GetComponentLocation().Z, CamTargetLocation.Z, DeltaTime, CurrentCamSmoothing);
	CameraComponent->SetWorldLocation(FVector(CamTargetLocation.X, CamTargetLocation.Y, CurrentZ));

	if (CameraComponent->GetComponentLocation().Z == CamTargetLocation.Z || GetCharacterMovement()->IsFalling())
	{
		CurrentCamSmoothing = 0.f;
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Actions
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction<FInputEventDelegate>(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::ToggleSprint, IE_Pressed);
	PlayerInputComponent->BindAction<FInputEventDelegate>(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::ToggleSprint, IE_Released);
	PlayerInputComponent->BindAction<FInputEventDelegate>(TEXT("Crouch"), IE_Pressed, this, &APlayerCharacter::ToggleCrouch, IE_Pressed);
	PlayerInputComponent->BindAction<FInputEventDelegate>(TEXT("Crouch"), IE_Released, this, &APlayerCharacter::ToggleCrouch, IE_Released);
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Pressed, this, &APlayerCharacter::OnAbilityActivated);
	PlayerInputComponent->BindAction(TEXT("AbilityUse"), IE_Released, this, &APlayerCharacter::OnAbilityUsed);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Pressed, this, &APlayerCharacter::OpenWeaponWheel);
	PlayerInputComponent->BindAction(TEXT("WeaponWheelUse"), IE_Released, this, &APlayerCharacter::CloseWeaponWheel);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APlayerCharacter::StartInteract);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Released, this, &APlayerCharacter::EndInteract);

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

void APlayerCharacter::ToggleCrouch(EInputEvent _InputEvent)
{
	if (bIsSliding || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	CurrentCamSmoothing = CameraSmoothing;
	
	if (bToggleCrouch)
	{
		if (_InputEvent == IE_Pressed)
		{
			if (bIsCrouched)
			{
				UnCrouch();
			}
			else
			{
				Crouch();

				if (bIsSprinting)
				{
					ToggleSlide();
				}
			}
		}
	}
	else
	{
		if (_InputEvent == IE_Pressed && !bIsCrouched)
		{
			Crouch();

			if (bIsSprinting)
			{
				ToggleSlide();
			}
		}
		else if (_InputEvent == IE_Released && bIsCrouched)
		{
			UnCrouch();
		}
	}
}

void APlayerCharacter::ToggleSprint(EInputEvent _InputEvent)
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
			CurrentCamSmoothing = CameraSmoothing;
		}

		if (bIsSliding)
		{
			ToggleSlide();
			GetWorld()->GetTimerManager().ClearTimer(SlidingHandle);
		}
	}
}

void APlayerCharacter::ToggleSlide()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		if (!bIsSliding && bCanSlide)
		{
			InitialGroundFriction = MoveComp->GroundFriction;
			InitialBrakingDeceleration = MoveComp->BrakingDecelerationWalking;
				
			MoveComp->GroundFriction = 0.f;
			MoveComp->BrakingDecelerationWalking = 0.f;
			
			GetWorld()->GetTimerManager().SetTimer(SlidingHandle, [this] {ToggleSlide();}, SlideTime, false);

			Controller->SetIgnoreMoveInput(true);
		}
		else
		{
			MoveComp->GroundFriction = InitialGroundFriction;
			MoveComp->BrakingDecelerationWalking = InitialBrakingDeceleration;

			Controller->SetIgnoreMoveInput(false);
		}
	}

	bIsSliding = !bIsSliding;
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
