// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_BendTime.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_BendTime::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (!PlayerPtr)
	{
		return;
	}

	Setup();
}

void UAb_BendTime::OnUse()
{
	Super::OnUse();
	
	if (!PlayerPtr)
	{
		return;
	}
	
	ToggleTimeBend();
}

void UAb_BendTime::OnDeactivation()
{
	Super::OnDeactivation();
}

void UAb_BendTime::Setup()
{
	if (StoredGlobalDilation < 0.f)
	{
		StoredGlobalDilation = UGameplayStatics::GetGlobalTimeDilation(PlayerPtr);
		StoredPlayerDilation = PlayerPtr->CustomTimeDilation;
	}

	Timeline.SetTimelineLengthMode(TL_TimelineLength);
	Timeline.SetTimelineLength(BendTimeLength);
		
	FOnTimelineEvent TimelineFinish;
	TimelineFinish.BindDynamic(this, &UAb_BendTime::ToggleTimeBend);
	Timeline.SetTimelineFinishedFunc(TimelineFinish);
}

void UAb_BendTime::Reset()
{
	UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, StoredGlobalDilation);
	PlayerPtr->CustomTimeDilation = StoredPlayerDilation;

	GetActorsCloseToPlayer();

	StoredGlobalDilation = -1.f;
	StoredPlayerDilation = -1.f;
	bIsBendingTime = false;

	Timeline.Stop();
}

void UAb_BendTime::ToggleTimeBend()
{
	bIsBendingTime = !bIsBendingTime;
	
	if (!bIsBendingTime) // bend end
	{
		Reset();
	}
	else // bend start
	{
		UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, GlobalDilation);

		const FVector PlayerVelocity = PlayerPtr->GetCharacterMovement()->Velocity;
		PlayerPtr->GetCharacterMovement()->SetMovementMode(MOVE_None);

		const FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UAb_BendTime::StartTimeBend, PlayerVelocity);
		PlayerPtr->GetWorld()->GetTimerManager().SetTimerForNextTick(EndDelegate);
	}
}

void UAb_BendTime::StartTimeBend(FVector _Velocity)
{
	PlayerPtr->CustomTimeDilation = (1.f / GlobalDilation) * PlayerDilation;
	PlayerPtr->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PlayerPtr->GetCharacterMovement()->Velocity = _Velocity;

	Timeline.PlayFromStart();
	TimerStart = FDateTime::Now();
	TickTimeline();
}

void UAb_BendTime::GetActorsCloseToPlayer()
{
	if (GlobalDilation > TimeStopThreshold)
	{
		return;
	}

	const TArray<AActor*> IgnoredActors = {PlayerPtr};
	
	if (bIsBendingTime)
	{
		TArray<AActor*> SimulatingActors;
		UKismetSystemLibrary::SphereOverlapActors(PlayerPtr, PlayerPtr->GetActorLocation(), SimulatingActorsRadius, {}, AActor::StaticClass(), IgnoredActors, SimulatingActors);
		
		// Store simulating actor + its' velocity, and turn off physics simulation for that actor
		for (int i = 0; i < SimulatingActors.Num(); i++)
		{
			if (SimulatingActors[i]->GetRootComponent()->IsSimulatingPhysics())
			{
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(SimulatingActors[i]->GetRootComponent()))
				{
					ActorsCloseToPlayer.Add(SimulatingActors[i], PrimComp->BodyInstance.GetUnrealWorldVelocity());
					PrimComp->SetSimulatePhysics(false);
				}
			}
		}
	}

	else // when not bending time, just reset all the stored actors
	{
		for (const auto Pair : ActorsCloseToPlayer)
		{
			if (IsValid(Pair.Key))
			{
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Pair.Key->GetRootComponent()))
				{
					PrimComp->SetSimulatePhysics(true);
					PrimComp->BodyInstance.SetLinearVelocity(Pair.Value, false);
				}
			}
		}

		ActorsCloseToPlayer.Empty();
	}
}

void UAb_BendTime::TickTimeline()
{
	Timeline.TickTimeline((FDateTime::Now() - TimerStart).GetTotalSeconds());
	TimerStart = FDateTime::Now();
	
	GetActorsCloseToPlayer();
	
	if (Timeline.IsPlaying())
	{
		PlayerPtr->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAb_BendTime::TickTimeline);
	}

	if (bDrawTimeRemaining)
	{
		const float TimeLeft = BendTimeLength - Timeline.GetPlaybackPosition();
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Bend Time Remaining: %f"), TimeLeft));
	}
}
