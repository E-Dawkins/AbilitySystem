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
	void MoveForward(float Axis);

	UFUNCTION()
	void MoveRight(float Axis);

private:
	// X => Forward, Y => Right
	UPROPERTY(EditAnywhere, Category = "Character")
	FVector2D MoveSpeeds = FVector2D(1, 1);

};
