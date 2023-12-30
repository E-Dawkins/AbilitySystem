// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DebugLog.h"

UBTTask_DebugLog::UBTTask_DebugLog()
{
	NodeName = TEXT("Debug Log");
}

EBTNodeResult::Type UBTTask_DebugLog::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (DebugType == Screen)
	{
		GEngine->AddOnScreenDebugMessage(LogIndex, TimeToDisplay, LogColor, DebugMessage);
	}
	else if (DebugType == OutputLog)
	{
		switch (Verbosity)
		{
			case Error:		UE_LOG(LogTemp, Error, TEXT("%s"), *DebugMessage);		break;
			case Warning:	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);	break;
			default:		UE_LOG(LogTemp, Display, TEXT("%s"), *DebugMessage);	break;
		}
	}
	else
	{
		Result = EBTNodeResult::Failed;
	}
	
	return Result;
}
