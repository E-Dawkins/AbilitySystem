// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_DevouringSwarm.h"

#include "AbilitySystem/Player/PlayerCharacter.h"

void UAb_DevouringSwarm::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (!PlayerPtr)
	{
		return;
	}

	if (UWorld* World = PlayerPtr->GetWorld())
	{
		if (SwarmCursor)
		{
			SwarmCursorPtr = World->SpawnActor(SwarmCursor);
			SwarmCursorPtr->SetActorHiddenInGame(true);
		}
	}
}

void UAb_DevouringSwarm::OnUse()
{
	Super::OnUse();

	OnDeactivation();
}

void UAb_DevouringSwarm::Update(float _DeltaSeconds)
{
	Super::Update(_DeltaSeconds);

	if (IsValid(SwarmCursorPtr))
	{
		FVector TraceLocation = PlayerPtr->GetActorLocation();
		const bool bLocation = GetTraceLocation(TraceLocation);

		SwarmCursorPtr->SetActorLocation(TraceLocation);
		SwarmCursorPtr->SetActorHiddenInGame(!bLocation);
	}
}

void UAb_DevouringSwarm::OnDeactivation()
{
	Super::OnDeactivation();

	if (IsValid(SwarmCursorPtr))
	{
		if (IsValid(Swarm) && IsValid(PlayerPtr))
		{
			if (IsValid(SwarmPtr))
			{
				SwarmPtr->Destroy();
			}
			
			SwarmPtr = PlayerPtr->GetWorld()->SpawnActor(Swarm);

			if (IsValid(SwarmPtr))
			{
				SwarmPtr->SetActorLocation(SwarmCursorPtr->GetActorLocation());
			}
		}
	}

	if (IsValid(SwarmCursorPtr))
	{
		SwarmCursorPtr->Destroy();
	}
}

bool UAb_DevouringSwarm::GetTraceLocation(FVector& _TraceLocation) const
{
	if (IsValid(PlayerPtr))
	{
		FVector ViewPoint;
		FRotator ViewDirection;
		PlayerPtr->GetActorEyesViewPoint(ViewPoint, ViewDirection);

		FHitResult OutHit;

		const bool bTraceSuccess = PlayerPtr->GetWorld()->SweepSingleByChannel
		(
			OutHit,
			ViewPoint,
			ViewPoint + ViewDirection.Vector() * CastReach,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SphereCastRadius)
		);

		if (bTraceSuccess)
		{
			_TraceLocation = OutHit.Location;
			return true;
		}
	}
	
	return false;
}
