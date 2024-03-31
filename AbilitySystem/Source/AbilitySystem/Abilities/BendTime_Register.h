// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BendTime_Register.generated.h"


class UBTRHandler_Base;
struct FTimeBendOptions;

USTRUCT(Atomic)
struct FHandlerOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bMeshHandler;

	UPROPERTY(EditAnywhere)
	bool bParticleHandler;

	UPROPERTY(EditAnywhere)
	bool bAIHandler;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBendTime_Register : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	void OnStartTimeSlow(FTimeBendOptions Options);
	void OnEndTimeSlow(FTimeBendOptions Options);
	
	void OnStartTimeStop(FTimeBendOptions Options);
	void OnEndTimeStop(FTimeBendOptions Options);

	void RunHandlers();
	void EndHandlers();

	void RegisterHandlers();
	UBTRHandler_Base* GetHandler(UActorComponent* ActorComp);

private:
	friend class UBendTime_Manager;

	UPROPERTY(EditAnywhere)
	bool bIgnoreTimeBend = false;

	UPROPERTY(EditAnywhere)
	FHandlerOptions HandlerOptions;

	UPROPERTY()
	TArray<UBTRHandler_Base*> HandlerPointers;

	/* Stored Data --> */

	float CustomDilation = 1.f;
	bool bActorTickEnabled = true;
	bool bSimulatePhysics = false;

	/* Stored Data <-- */
	
};
