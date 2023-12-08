// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_BendTime.h"
#include <Kismet/GameplayStatics.h>

#include "GameFramework/CharacterMovementComponent.h"

void UAb_BendTime::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (StoredGlobalDilation < 0.f)
	{
		StoredGlobalDilation = UGameplayStatics::GetGlobalTimeDilation(PlayerPtr);
		StoredPlayerDilation = PlayerPtr->CustomTimeDilation;
	}
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
	if (bIsBendingTime)
	{
		GetActorsCloseToPlayer();
		
		if ((FDateTime::Now() - TimerStart).GetTotalSeconds() >= BendTimeLength)
		{
			ToggleTimeBend();
		}
	}
	
	if (bDrawTimeRemaining)
	{
		if (bIsBendingTime)
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Green, FString::Printf(TEXT("Bend Time Remaining: %f"), BendTimeLength - (FDateTime::Now() - TimerStart).GetTotalSeconds()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, FString::Printf(TEXT("Bend Time Not Active")));
		}
	}
}

void UAb_BendTime::OnDeactivation()
{
	Super::OnDeactivation();

	UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, StoredGlobalDilation);
	PlayerPtr->CustomTimeDilation = StoredPlayerDilation;

	GetActorsCloseToPlayer();

	StoredGlobalDilation = -1.f;
	StoredPlayerDilation = -1.f;
}

void UAb_BendTime::ToggleTimeBend()
{
	bIsBendingTime = !bIsBendingTime;
	
	if (!bIsBendingTime) // bend end
	{
		OnDeactivation();
	}
	else // bend start
	{
		UGameplayStatics::SetGlobalTimeDilation(PlayerPtr, GlobalDilation);

		const FVector PlayerVelocity = PlayerPtr->GetCharacterMovement()->Velocity;
		PlayerPtr->GetCharacterMovement()->SetMovementMode(MOVE_None);

		const FTimerDelegate EndDelegate = FTimerDelegate::CreateUObject(this, &UAb_BendTime::StartTimeBend, PlayerVelocity);
		PlayerPtr->GetWorld()->GetTimerManager().SetTimerForNextTick(EndDelegate);
		
		TimerStart = FDateTime::Now();
	}
}

void UAb_BendTime::StartTimeBend(FVector _Velocity) const
{
	PlayerPtr->CustomTimeDilation = (1.f / GlobalDilation) * PlayerDilation;
	PlayerPtr->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PlayerPtr->GetCharacterMovement()->Velocity = _Velocity;
}

void UAb_BendTime::GetActorsCloseToPlayer()
{
	if (GlobalDilation > TimeStopThreshold)
	{
		return;
	}
	
	TArray<AActor*> SimulatingActors;
	
	if (bIsBendingTime)
	{
		TArray<FHitResult> OutHits;
		
		PlayerPtr->GetWorld()->SweepMultiByChannel
	   (
		   OutHits,
		   PlayerPtr->GetActorLocation(),
		   PlayerPtr->GetActorLocation(),
		   FQuat::Identity,
		   ECC_WorldDynamic,
		   FCollisionShape::MakeCapsule(PlayerPtr->GetSimpleCollisionCylinderExtent() * 1.25f)
	   );

		// Filter out the simulating actors
		for (const FHitResult& Hit : OutHits)
		{
			if (Hit.GetActor()->GetRootComponent()->IsSimulatingPhysics())
			{
				SimulatingActors.AddUnique(Hit.GetActor());
			}
		}

		// Store actor + its' data, and stop simulating physics on that actor
		for (AActor*& Actor : SimulatingActors)
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
			{
				const FBodyInstance BodyInstance = PrimComp->BodyInstance;
				const FVector Velocity = BodyInstance.GetUnrealWorldVelocity();
				ActorsCloseToPlayer.Add(Actor, FFrozenActorData{BodyInstance, Velocity});
			
				PrimComp->SetSimulatePhysics(false);
				PrimComp->BodyInstance.bLockXTranslation = true;
				PrimComp->BodyInstance.bLockYTranslation = true;
				PrimComp->BodyInstance.bLockZTranslation = true;
				PrimComp->BodyInstance.bLockXRotation = true;
				PrimComp->BodyInstance.bLockYRotation = true;
				PrimComp->BodyInstance.bLockZRotation = true;
			}
		}
	}

	else // when not bending time, just reset all the stored actors
	{
		TArray<AActor*> Keys;
		ActorsCloseToPlayer.GetKeys(Keys);
	
		for (int i = ActorsCloseToPlayer.Num() - 1; i >= 0; i--)
		{
			if (!SimulatingActors.Contains(Keys[i]))
			{
				if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Keys[i]->GetRootComponent()))
				{
					PrimComp->SetSimulatePhysics(true);
					PrimComp->BodyInstance = ActorsCloseToPlayer[Keys[i]].BodyInstance;
					PrimComp->BodyInstance.SetLinearVelocity(ActorsCloseToPlayer[Keys[i]].Velocity, false);
				}
				
				ActorsCloseToPlayer.Remove(Keys[i]);
				Keys.RemoveAt(i);
			}
		}
	}
}
