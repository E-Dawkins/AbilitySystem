// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Ab_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEM_API UAb_Teleport : public UBaseAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivation(UWorld* World) override;
	virtual void OnUse(APlayerCharacter* Player) override;
	virtual void Update(APlayerCharacter* Player, float DeltaSeconds) override;
	virtual void OnDeactivation() override;

private:
	bool GetTeleportLocation(APlayerCharacter* Player, FVector& TeleportLocation);

private:
	UPROPERTY(EditAnywhere, Category = "Teleport")
	TSubclassOf<AActor> Cursor;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector CursorScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FRotator CursorRotation;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportRange = 500.f;

	AActor* CursorPtr;
	
};
