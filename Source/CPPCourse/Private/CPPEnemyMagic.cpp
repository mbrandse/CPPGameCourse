// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPEnemyMagic.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "CPPPlayerCharacter.h"
#include "CPPCourse/CPPCourse.h"

ACPPEnemyMagic::ACPPEnemyMagic()
{
	BrainUpdateInterval = 1.f;
	bAllowFriendlyFire = false;
	DetectionRange = 2200.f;
	DesiredRotation = FRotator::ZeroRotator;
}

void ACPPEnemyMagic::BeginPlay()
{
	Super::BeginPlay();

	if(GetWorld())
	{
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UCPPEnemyInterface::StaticClass(), IgnoreActors);
		GetWorld()->GetTimerManager().SetTimer(BrainTimerHandle, this, &ACPPEnemyMagic::BrainUpdate, BrainUpdateInterval, true);
		BrainUpdate();
	}

	bIsFiring = false;
}

void ACPPEnemyMagic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRot;

	if (Target)
	{
		FRotator TempRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		DesiredRotation.Yaw = TempRot.Yaw; //we only need the yaw data, otherwise we get really weird rotations
	}

	FRotator CurrentRot = GetActorRotation();
	NewRot = FMath::RInterpTo(CurrentRot, DesiredRotation, DeltaTime, 2.f);

	SetActorRotation(NewRot.Quaternion());
}

void ACPPEnemyMagic::BrainUpdate()
{
	if(GetWorld()
		&& !bIsFiring)
	{
		UE_LOG(AnyLog, Log, TEXT("CALCULATING WHETHER CAN FIRE"));
		//first set the random rotation, but only if we have no target
		if (!Target)
		{
			DesiredRotation.Yaw = FMath::RandRange(0.f, 360.f);
		}

		//first see if we have a line of sight to the player, though we want to exclude friendlies.
		ACPPPlayerCharacter* Player = Cast<ACPPPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if(!Player) return; //no use continueing if we don't have a player to begin with.

		if(FVector::Dist(GetActorLocation(), Player->GetActorLocation()) >= DetectionRange)
			return; //return if we are too far away as well.

		FCollisionQueryParams Params;
		Params.AddIgnoredActors(IgnoreActors);
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), Player->GetActorLocation(), ECC_Camera, Params);
		if (bHit
			&& HitResult.GetActor() == Player)
		{
			Target = Player;
			StartMagic();
			bIsFiring = true;
		}
		else
		{
			UE_LOG(AnyLog, Log, TEXT("NO TARGET"));
			Target = nullptr; //null it if we don't have a hit, so we can return back to normalcy
		}
	}
}

void ACPPEnemyMagic::StartMagic()
{
	OnReceiveStartMagic();
}

void ACPPEnemyMagic::FireMagic()
{
	OnReceiveFireMagic();
}

void ACPPEnemyMagic::FinishMagic()
{
	bIsFiring = false;
}