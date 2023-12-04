// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_BendTime.h"
#include <Kismet/GameplayStatics.h>

void UAb_BendTime::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	StoredGlobalDilation = UGameplayStatics::GetGlobalTimeDilation(PlayerPtr);
	StoredPlayerDilation = PlayerPtr->CustomTimeDilation;
}

void UAb_BendTime::OnUse()
{
	if (!PlayerPtr)
	{
		return;
	}
	
	ToggleTimeBend();
}

void UAb_BendTime::Update(float _DeltaSeconds)
{
	if (bDrawTimeRemaining)
	{
		const float TimerRemaining = PlayerPtr->GetWorld()->GetTimerManager().GetTimerRemaining(BendTimeHandle);

		if (TimerRemaining >= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Bend Time Remaining: %f"), TimerRemaining));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, FString::Printf(TEXT("Bend Time Not Active")));
		}
	}
}

void UAb_BendTime::ToggleTimeBend()
{
	if (bIsBendingTime)
	{
		UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, StoredGlobalDilation);
		PlayerPtr->CustomTimeDilation = StoredPlayerDilation;

		PlayerPtr->GetWorld()->GetTimerManager().ClearTimer(BendTimeHandle);
	}
	else
	{
		// Set a timer to auto end after BendTimeLength seconds
		PlayerPtr->GetWorld()->GetTimerManager().SetTimer(BendTimeHandle, this, &UAb_BendTime::ToggleTimeBend, BendTimeLength * GlobalDilation, false);
		
		UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, GlobalDilation);
		PlayerPtr->CustomTimeDilation = (1.f / GlobalDilation) * PlayerDilation;
	}

	bIsBendingTime = !bIsBendingTime;
}
