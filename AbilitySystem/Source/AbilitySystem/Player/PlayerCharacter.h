// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Abilities/BaseAbility.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class ABaseInteractable;
class UPlayerHUD;
class UWeaponWheel;

UCLASS(hideCategories = (Replication, Rendering, Actor, LOD, Cooking, ActorTick))
class ABILITYSYSTEM_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

private:
	DECLARE_DELEGATE_OneParam(FInputEventDelegate, EInputEvent)
	
	void MoveForward(float _Value);
	void MoveRight(float _Value);
	void ToggleCrouch(EInputEvent _InputEvent);
	void ToggleSprint(EInputEvent _InputEvent);
	void ToggleSlide();
	void OnAbilityUsed();
	void OnAbilityActivated();
	void OpenWeaponWheel();
	void CloseWeaponWheel();
	void StartInteract();
	void EndInteract();

	void CheckForInteractable();

public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentAbility(TSubclassOf<UBaseAbility> _NewAbility);

	UFUNCTION(BlueprintCallable)
	UBaseAbility* GetCurrentAbility() const { return CurrentAbility.GetDefaultObject(); }

	UFUNCTION(BlueprintCallable)
	float GetInteractionRange() const { return InteractionRange; }

private:
	UPROPERTY(EditAnywhere, Category = "Player|Movement") // In cm/s
	float MaxWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category = "Player|Movement") // In cm/s
	float MaxSprintSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Player|Movement") // In cm/s
	float MaxCrouchSpeed = 300.f;

	// Toggle means press to start sprint, press again to end sprint
	UPROPERTY(EditAnywhere, Category = "Player|Movement")
	bool bToggleSprint = false;

	// Toggle means press to start crouch, press again to end crouch
	UPROPERTY(EditAnywhere, Category = "Player|Movement")
	bool bToggleCrouch = true;

	// Slide is activated by crouching while sprinting.
	// Slide can be cancelled by starting to sprint mid-slide.
	// Slide will only activate if velocity is >= mid-point of walk / sprint speeds.
	UPROPERTY(EditAnywhere, Category = "Player|Movement")
	bool bCanSlide = true;

	// The length of the slide, in seconds
	UPROPERTY(EditAnywhere, Category = "Player|Movement", meta=(ClampMin = "0.1", EditCondition = "bCanSlide", EditConditionHides))
	float SlideTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Player|UI")
	TSubclassOf<UWeaponWheel> WeaponWheelClass;

	UPROPERTY(EditAnywhere, Category = "Player|UI")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere, Category = "Player")
	TSubclassOf<UBaseAbility> CurrentAbility;

	UPROPERTY(EditAnywhere, Category = "Player")
	float InteractionRange = 300.f;

	// If this is 0, instantly set camera location (no smoothing)
	UPROPERTY(EditAnywhere, Category = "Player", meta=(ClampMin = "0.0"))
	float CameraSmoothing = 10.f;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

private:
	bool bIsSprinting = false;
	bool bIsSliding = false;
	bool bShouldCrouch = false;

	float InitialGroundFriction = 0.f;
	float InitialBrakingDeceleration = 0.f;
	
	float CamSmoothing_Current = 0.f;
	FVector CamSmoothing_Offset = FVector::ZeroVector;

	FTimerHandle SlidingHandle;

	UPROPERTY()
	UWeaponWheel* WeaponWheelPtr = nullptr;

	UPROPERTY()
	UPlayerHUD* PlayerHUDPtr = nullptr;

	UPROPERTY()
	ABaseInteractable* CurrentInteractable = nullptr;

};
