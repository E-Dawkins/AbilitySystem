// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_FarReach.h"

#include "DrawDebugHelpers.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "AbilitySystem/_Misc/Helpers.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_FarReach::OnUse()
{
	Super::OnUse();

	if (!PlayerPtr)
	{
		return;
	}

	// Disable player input / collision temporarily
	PlayerPtr->DisableInput(PlayerPtr->GetController<APlayerController>());
	PlayerPtr->SetActorEnableCollision(false);
	
	if (bShouldCrouch)
	{
		PlayerPtr->Crouch();
	}

	VelocityBeforeTp = PlayerPtr->GetVelocity();
	
	PlayerPtr->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	PlayerPtr->GetCharacterMovement()->GravityScale = 0;
	
	bIsSweeping = true;
}

void UAb_FarReach::Update(float _DeltaSeconds)
{
	if (bIsSweeping)
	{
		SweepTowards();
	}
	else
	{
		Super::Update(_DeltaSeconds);
	}
}

bool UAb_FarReach::InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit, APlayerCharacter* _PlayerPtr) const
{
	FVector TraceStart;
	FRotator TraceDirection;
	PlayerPtr->GetActorEyesViewPoint(TraceStart, TraceDirection);

	const bool bInitialTraceResult = PlayerPtr->GetWorld()->SweepSingleByChannel
	(
		_TraceHit,
		TraceStart,
		TraceStart + PlayerPtr->GetControlRotation().Vector() * TeleportRange,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InitialTraceRadius)
	);

	if (!bInitialTraceResult) // no trace hit, tp location is mid-air
	{
		_TeleportLocation = _TraceHit.TraceEnd - TraceDirection.Vector() * PlayerPtr->GetSimpleCollisionRadius()
								+ FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	}

	if (bDrawInitialTraceHit)
	{
		DrawDebugSphere(PlayerPtr->GetWorld(), bInitialTraceResult ? _TraceHit.ImpactPoint : _TraceHit.TraceEnd, 10.f, 12, FColor::Black);
	}
	
	return bInitialTraceResult;
}

void UAb_FarReach::SweepTowards()
{
	if (!IsValid(PlayerPtr))
	{
		return;
	}
	
	PlayerPtr->SetActorLocation(FHelpers::MoveTowards(PlayerPtr->GetActorLocation(), TeleportLocation, SweepStepSize));
	
	if (PlayerPtr->GetActorLocation().Equals(TeleportLocation)) // reached tp location
	{
		bIsSweeping = false;
		PlayerPtr->GetCharacterMovement()->GravityScale = 1;
		
		const float ClampedVelocity = FMath::Clamp(VelocityBeforeTp.Size(), MinExitVelocity, MaxExitVelocity);
		PlayerPtr->GetCharacterMovement()->Velocity = ClampedVelocity * PlayerPtr->GetControlRotation().Vector();

		OnDeactivation();
	}
}
