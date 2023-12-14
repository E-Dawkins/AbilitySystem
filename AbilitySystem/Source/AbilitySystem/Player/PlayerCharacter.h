// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Abilities/BaseAbility.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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

private:
	void MoveForward(float _Value);
	void MoveRight(float _Value);
	void ToggleCrouch();
	void ToggleSprint();
	void OnAbilityUsed();
	void OnAbilityActivated();
	void OpenWeaponWheel();
	void CloseWeaponWheel();

public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentAbility(TSubclassOf<UBaseAbility> _NewAbility) { CurrentAbility = _NewAbility; }

	UFUNCTION(BlueprintCallable)
	UBaseAbility* GetCurrentAbility() const { return CurrentAbility.GetDefaultObject(); }

	UFUNCTION(BlueprintCallable)
	UPlayerHUD* GetPlayerHUD() const {return PlayerHUDPtr; }

private:
	UPROPERTY(EditAnywhere, Category = "Character") // In cm/s
	float MaxWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category = "Character") // In cm/s
	float MaxSprintSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Character") // In cm/s
	float MaxCrouchSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Character")
	TSubclassOf<UBaseAbility> CurrentAbility;

	UPROPERTY(EditAnywhere, Category = "Character")
	TSubclassOf<UWeaponWheel> WeaponWheelClass;

	UPROPERTY(EditAnywhere, Category = "Character")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

private:
	bool bIsSprinting = false;

	UPROPERTY()
	UWeaponWheel* WeaponWheelPtr = nullptr;

	UPROPERTY()
	UPlayerHUD* PlayerHUDPtr = nullptr;

};
