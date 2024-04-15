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

	UPROPERTY(EditAnywhere)
	bool bAnimHandler;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYSYSTEM_API UBendTime_Register : public UActorComponent
{
	GENERATED_BODY()

	UBendTime_Register();
	
protected:
	virtual void BeginPlay() override;

private:
	void OnStartTimeSlow(FTimeBendOptions Options);
	void OnEndTimeSlow(FTimeBendOptions Options) const;
	
	void OnStartTimeStop(FTimeBendOptions Options);
	void OnEndTimeStop(FTimeBendOptions Options);

	void RunHandlers();
	void EndHandlers();

	void RegisterHandlers();
	TArray<UBTRHandler_Base*> GetHandlers(const UActorComponent* ActorComp);

private:
	friend class UBendTime_Manager;

	UPROPERTY(EditAnywhere)
	bool bIgnoreTimeBend;

	UPROPERTY(EditAnywhere)
	FHandlerOptions HandlerOptions;

	UPROPERTY()
	TArray<UBTRHandler_Base*> HandlerPointers;

	/* Stored Data --> */

	float CustomDilation;
	bool bActorTickEnabled;
	bool bSimulatePhysics;

	/* Stored Data <-- */
	
};
