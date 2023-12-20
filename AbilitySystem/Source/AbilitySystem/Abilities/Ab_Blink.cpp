// Fill out your copyright notice in the Description page of Project Settings.


#include "Ab_Blink.h"

#include "DrawDebugHelpers.h"
#include "AbilitySystem/Player/PlayerCharacter.h"
#include "AbilitySystem/_Misc/Helpers.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAb_Blink::OnActivation(APlayerCharacter* _Player)
{
	Super::OnActivation(_Player);

	if (!PlayerPtr)
	{
		return;
	}

	// Set default teleport / cursor locations
	TeleportLocation = PlayerPtr->GetActorLocation();
	CursorLocation = TeleportLocation - FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	
	// Spawn the teleport cursors, so player knows where they are teleporting
	if (const auto World = PlayerPtr->GetWorld())
	{
		NormalCursorPtr = World->SpawnActor(NormalCursor);
		LedgeCursorPtr = World->SpawnActor(LedgeCursor);
		CrouchCursorPtr = World->SpawnActor(CrouchCursor);

		NormalCursorPtr->SetActorHiddenInGame(true);
		LedgeCursorPtr->SetActorHiddenInGame(true);
		CrouchCursorPtr->SetActorHiddenInGame(true);
	}
}

void UAb_Blink::OnUse()
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
	
	PlayerPtr->SetActorLocation(TeleportLocation);
	PlayerPtr->GetCharacterMovement()->Velocity = FVector::ZeroVector;

	OnDeactivation();
}

void UAb_Blink::Update(float _DeltaSeconds)
{
	Super::Update(_DeltaSeconds);

	if (!PlayerPtr)
	{
		return;
	}

	UpdateTeleportVariables();
	
	if (NormalCursorPtr)
	{
		NormalCursorPtr->SetActorLocation(CursorLocation);
		NormalCursorPtr->SetActorHiddenInGame(bCanMantle || bShouldCrouch);
	}

	if (LedgeCursorPtr)
	{
		LedgeCursorPtr->SetActorLocation(CursorLocation);
		LedgeCursorPtr->SetActorHiddenInGame(!bCanMantle || bShouldCrouch);
	}

	if (CrouchCursorPtr)
	{
		CrouchCursorPtr->SetActorLocation(CursorLocation);
		CrouchCursorPtr->SetActorHiddenInGame(bCanMantle || !bShouldCrouch);
	}
}

void UAb_Blink::OnDeactivation()
{
	Super::OnDeactivation();

	if (IsValid(NormalCursorPtr))
	{
		NormalCursorPtr->Destroy();
	}

	if (IsValid(LedgeCursorPtr))
	{
		LedgeCursorPtr->Destroy();
	}

	if (IsValid(CrouchCursorPtr))
	{
		CrouchCursorPtr->Destroy();
	}

	if (PlayerPtr)
	{
		// Re-enable player input / collision
		PlayerPtr->EnableInput(PlayerPtr->GetController<APlayerController>());
		PlayerPtr->SetActorEnableCollision(true);
	}
}

void UAb_Blink::UpdateTeleportVariables()
{
	bCanMantle = false;
	bShouldCrouch = false;
	
	FVector FinalTpLocation = TeleportLocation;
	FHitResult InitialTraceHit;

	const bool bInitialTrace = InitialTrace(FinalTpLocation, InitialTraceHit);
	
	if (bInitialTrace)
	{
		MantleTrace(InitialTraceHit, FinalTpLocation);
	}
	
	if (CheckHeadRoom(FinalTpLocation))
	{
		TeleportLocation = FinalTpLocation;
		CursorLocation = TeleportLocation - FVector::UpVector * PlayerPtr->GetSimpleCollisionHalfHeight();
	}

	if (bDrawStats)
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, bCanMantle ? FColor::Green : FColor::Red, FString("Can Mantle"));
		GEngine->AddOnScreenDebugMessage(-1, -1, bShouldCrouch ? FColor::Green : FColor::Red, FString("Should Crouch"));
	}
}

bool UAb_Blink::InitialTrace(FVector& _TeleportLocation, FHitResult& _TraceHit) const
{
	FVector TraceStart;
	FRotator TraceDirection;
	PlayerPtr->GetActorEyesViewPoint(TraceStart, TraceDirection);

	const bool bInitialTraceResult = PlayerPtr->GetWorld()->LineTraceSingleByChannel
	(
		_TraceHit,
		TraceStart,
		TraceStart + PlayerPtr->GetControlRotation().Vector() * TeleportRange,
		ECC_Visibility
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

void UAb_Blink::MantleTrace(const FHitResult _InitialTraceHit, FVector& _TeleportLocation)
{
	const bool bMantleAble = FMath::Abs(FVector::DotProduct(FVector::UpVector, _InitialTraceHit.ImpactNormal)) <= WallDotTolerance;

	TArray<FHitResult> HitResultArr;
	
	if (bMantleAble)
	{
		FVector NormalLocalAxis[2];
		FHelpers::GetLocalAxisFromForward(_InitialTraceHit.ImpactNormal, NormalLocalAxis[0], NormalLocalAxis[1]);

		FHelpers::RecursiveSphereTrace
		(
				PlayerPtr,
				_InitialTraceHit.ImpactPoint,
				_InitialTraceHit.ImpactPoint,
				_InitialTraceHit.ImpactPoint + NormalLocalAxis[1] * EdgeTolerance * 2.f,
				MantleTraceRadius,
				HitResultArr
		);

		bCanMantle = FVector::Dist(_InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint) <= EdgeTolerance;
	}

	if (bCanMantle)
	{
		const float HeightPlusTolerance = PlayerPtr->GetSimpleCollisionHalfHeight() + WallTolerance * 2.f;
		_TeleportLocation = HitResultArr.Last().ImpactPoint + FVector::UpVector * HeightPlusTolerance;
	}
	else
	{
		const FVector ExtentPlusTolerance = PlayerPtr->GetSimpleCollisionCylinderExtent() + FVector(WallTolerance);
		_TeleportLocation = _InitialTraceHit.ImpactPoint + _InitialTraceHit.ImpactNormal * ExtentPlusTolerance;
	}

	if (bDrawMantle && HitResultArr.Num() > 0)
	{
		DrawDebugLine(PlayerPtr->GetWorld(), _InitialTraceHit.ImpactPoint, HitResultArr.Last().ImpactPoint, MantleLocalUp);

		for (FHitResult& Hit : HitResultArr)
		{
			DrawDebugSphere(PlayerPtr->GetWorld(), Hit.ImpactPoint, MantleTraceRadius, 12, bCanMantle ? MantleTraceSucceed : MantleTraceFail);
		}
	}
}

bool UAb_Blink::CheckHeadRoom(FVector& _TeleportLocation)
{
	FVector DepenetrationVector;
	const FVector TpLocationNoOffset = _TeleportLocation;
	
	const float PlayerHalfHeight = PlayerPtr->GetSimpleCollisionHalfHeight();
	const float PlayerCrouchedHalfHeight = PlayerPtr->GetCharacterMovement()->CrouchedHalfHeight;

	const FDebugHelper DebugHelper = {bDrawHeadRoom, HeadRoomSucceed, HeadRoomFail};
	
	bool bCanTeleport = FHelpers::CheckPlayerHeadRoom(PlayerPtr, _TeleportLocation,
		Standing, DebugHelper, DepenetrationVector, DepenetrationPadding); // standing check #1
	
	if (!bCanTeleport)
	{
		_TeleportLocation += DepenetrationVector;
		bCanTeleport = FHelpers::CheckPlayerHeadRoom(PlayerPtr, _TeleportLocation,
			Standing, DebugHelper, DepenetrationVector, DepenetrationPadding); // standing check #2

		if (!bCanTeleport && !PlayerPtr->bIsCrouched)
		{
			_TeleportLocation = TpLocationNoOffset - FVector::UpVector * (PlayerHalfHeight - PlayerCrouchedHalfHeight);
			bShouldCrouch = FHelpers::CheckPlayerHeadRoom(PlayerPtr, _TeleportLocation,
				Crouched, DebugHelper, DepenetrationVector, DepenetrationPadding); // crouch check #1

			if (!bShouldCrouch)
			{
				_TeleportLocation += DepenetrationVector;
				bShouldCrouch = FHelpers::CheckPlayerHeadRoom(PlayerPtr, _TeleportLocation,
					Crouched, DebugHelper, DepenetrationVector, DepenetrationPadding); // crouch check #2
			}

			bCanTeleport = bShouldCrouch;
		}
	}

	return bCanTeleport;
}
