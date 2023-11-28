// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS(hideCategories = (Replication, Rendering, Actor, LOD, Cooking, ActorTick))
class ABILITYSYSTEM_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void MoveForward(float _Value);

	UFUNCTION()
	void MoveRight(float _Value);

	UFUNCTION()
	void ToggleCrouch();

	UFUNCTION()
	void ToggleSprint();

	UFUNCTION()
	void OnAbilityUsed();

private:
	// In cm/s
	UPROPERTY(EditAnywhere, Category = "Character")
	float MaxWalkSpeed = 600.f;

	// In cm/s
	UPROPERTY(EditAnywhere, Category = "Character")
	float MaxSprintSpeed = 1000.f;

	// In cm/s
	UPROPERTY(EditAnywhere, Category = "Character")
	float MaxCrouchSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Character")
	TSubclassOf<class UBaseAbility> CurrentAbility;

	bool bIsSprinting = false;

};
