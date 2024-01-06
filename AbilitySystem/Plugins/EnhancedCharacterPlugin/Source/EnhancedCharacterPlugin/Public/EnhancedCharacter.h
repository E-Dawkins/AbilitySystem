// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedCharacter.generated.h"

class UCameraComponent;

UCLASS()
class ENHANCEDCHARACTERPLUGIN_API AEnhancedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnhancedCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	DECLARE_DELEGATE_OneParam(FInputEventDelegate, EInputEvent)
	
	void MoveForward(float _Value);
	void MoveRight(float _Value);

	void ToggleSprint(EInputEvent _InputEvent);
	void ToggleCrouch(EInputEvent _InputEvent);
	void ToggleSlide();

protected:
	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Axis")
	FString MoveForwardMapping = "MoveForward";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Axis")
	FString MoveRightMapping = "MoveRight";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Axis")
	FString LookUpMapping = "LookUp";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Axis")
	FString LookRightMapping = "LookRight";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Actions")
	FString JumpMapping = "Jump";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Actions")
	FString SprintMapping = "Sprint";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character|Mappings|Actions")
	FString CrouchMapping = "Crouch";

	UPROPERTY(EditAnywhere, Category = "Enhanced Character") // In cm/s
	float MaxWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category = "Enhanced Character") // In cm/s
	float MaxSprintSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Enhanced Character") // In cm/s
	float MaxCrouchSpeed = 300.f;

	// Toggle means press to start sprint, press again to end sprint
	UPROPERTY(EditAnywhere, Category = "Enhanced Character")
	bool bToggleSprint = false;

	// Toggle means press to start crouch, press again to end crouch
	UPROPERTY(EditAnywhere, Category = "Enhanced Character")
	bool bToggleCrouch = true;

	// Slide is activated by crouching while sprinting.
	// Slide can be cancelled by starting to sprint mid-slide.
	// Slide will only activate if velocity is >= mid-point of walk / sprint speeds.
	UPROPERTY(EditAnywhere, Category = "Enhanced Character")
	bool bCanSlide = true;

	// The length of the slide, in seconds
	UPROPERTY(EditAnywhere, Category = "Enhanced Character", meta=(ClampMin = "0.1", EditCondition = "bCanSlide", EditConditionHides))
	float SlideTime = 0.5f;

	// If this is 0, instantly set camera location (no smoothing)
	UPROPERTY(EditAnywhere, Category = "Enhanced Character", meta=(ClampMin = "0.0"))
	float CameraSmoothing = 10.f;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent = nullptr;
	
protected:
	bool bIsSprinting = false;
	bool bIsSliding = false;
	bool bShouldCrouch = false;

	float InitialGroundFriction = 0.f;
	float InitialBrakingDeceleration = 0.f;
	
	float CamSmoothing_Current = 0.f;
	FVector CamSmoothing_Offset = FVector::ZeroVector;

	FTimerHandle SlidingHandle;

};
