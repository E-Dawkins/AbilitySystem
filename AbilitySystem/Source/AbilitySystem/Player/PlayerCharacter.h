// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedCharacter.h"
#include "GameFramework/Character.h"
#include "../Abilities/BaseAbility.h"
#include "PlayerCharacter.generated.h"

class UBaseAbility;
class ABaseInteractable;
class UPlayerHUD;
class UWeaponWheel;

UCLASS()
class ABILITYSYSTEM_API APlayerCharacter : public AEnhancedCharacter
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<UWeaponWheel> WeaponWheelClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBaseAbility> CurrentAbility;

	UPROPERTY(EditAnywhere)
	float InteractionRange = 300.f;

private:
	UPROPERTY()
	UWeaponWheel* WeaponWheelPtr = nullptr;

	UPROPERTY()
	UPlayerHUD* PlayerHUDPtr = nullptr;

	UPROPERTY()
	ABaseInteractable* CurrentInteractable = nullptr;

};
