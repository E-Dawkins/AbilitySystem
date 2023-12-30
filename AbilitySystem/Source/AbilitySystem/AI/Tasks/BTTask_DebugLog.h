// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DebugLog.generated.h"

UENUM()
enum EDebugType
{
	Screen,
	OutputLog
};

UENUM()
enum EVerbosity
{
	Display, // White text
	Warning, // Yellow text
	Error // Red text
};

/**
 * Used to log a message either to the screen or to the output log.
 */
UCLASS()
class ABILITYSYSTEM_API UBTTask_DebugLog : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DebugLog();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Debug Log")
	FString DebugMessage = "Default";

	UPROPERTY(EditAnywhere, Category = "Debug Log")
	TEnumAsByte<EDebugType> DebugType = Screen;

	// If -1, will print newest log on top, otherwise will over-write log at that index.
	UPROPERTY(EditAnywhere, Category = "Debug Log", meta=(EditCondition="DebugType==EDebugType::Screen", EditConditionHides))
	int32 LogIndex = -1;
	
	// If -1, will last 1 frame, otherwise will be in seconds.
	UPROPERTY(EditAnywhere, Category = "Debug Log", meta=(EditCondition="DebugType==EDebugType::Screen", EditConditionHides))
	float TimeToDisplay = -1;

	UPROPERTY(EditAnywhere, Category = "Debug Log", meta=(EditCondition="DebugType==EDebugType::Screen", EditConditionHides))
	FColor LogColor = FColor::Purple;

	UPROPERTY(EditAnywhere, Category = "Debug Log", meta=(EditCondition="DebugType==EDebugType::OutputLog", EditConditionHides))
	TEnumAsByte<EVerbosity> Verbosity = Warning;
	
};
