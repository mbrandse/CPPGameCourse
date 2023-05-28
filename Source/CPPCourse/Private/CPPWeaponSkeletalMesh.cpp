// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPWeaponSkeletalMesh.h"
#include "CPPCourse/CPPCourseCharacter.h"
#include "CPPCourse/CPPCourse.h"
#include "Kismet/GameplayStatics.h"
#include "CPPEnemyInterface.h"
#include "CPPPlayerCharacter.h"

UCPPWeaponSkeletalMesh::UCPPWeaponSkeletalMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bTraceAttack = false;
}

void UCPPWeaponSkeletalMesh::BeginPlay()
{
	SetComponentTickEnabled(false); //since for some reason, the bStartWithTickEnabled doesnt work right now.

	WeaponOwner = Cast<ACPPCourseCharacter>(GetOwner());
	if (WeaponOwner)
	{
		WeaponOwner->OnStartAttack.AddDynamic(this, &UCPPWeaponSkeletalMesh::OnStartAttack);
		WeaponOwner->OnStopAttack.AddDynamic(this, &UCPPWeaponSkeletalMesh::OnStopAttack);
		WeaponOwner->OnToggleAttackTrace.AddDynamic(this, &UCPPWeaponSkeletalMesh::OnToggleAttackTrace);
	}

	Super::BeginPlay();
}

void UCPPWeaponSkeletalMesh::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTraceAttack
		&& GetWorld()
		&& DoesSocketExist(TraceStartSocketName)
		&& DoesSocketExist(TraceEndSocketName))
	{
		FVector TraceStart = GetSocketLocation(TraceStartSocketName);
		FVector TraceEnd = GetSocketLocation(TraceEndSocketName);
		FHitResult HitResult;

		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.f);

		//Default class implementation
		/*bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera);
		if (bHit && CanAttackActor(HitResult.GetActor()))
		{
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, WeaponOwner->GetController(), WeaponOwner, DamageClass);

			ICPPEnemyInterface* EnemyInterface = Cast<ICPPEnemyInterface>(HitResult.GetActor());
			ACPPPlayerCharacter* Player = Cast<ACPPPlayerCharacter>(GetWeaponOwner());

			if (EnemyInterface
				&& Player
				&& EnemyInterface->Execute_IsDead(HitResult.GetActor()))
			{
				Player->RemoveTarget(HitResult.GetActor());
			}
		}*/

		//Assingment implementation (one possibility)
		float TraceDistance = (FVector::Dist(TraceStart, TraceEnd)) / 5.f;
		FVector TraceDirection = (TraceEnd - TraceStart).GetSafeNormal();

		for (int32 i = 0; i < 5; i++)
		{
			if (PreviousTraceLocs.Num() < 5)
			{
				PreviousTraceLocs.Add(TraceStart + (TraceDirection * (TraceDistance * (float)i)));
			}
			else
			{
				FVector NewTrace = TraceStart + (TraceDirection * (TraceDistance * (float)i));
				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, PreviousTraceLocs[i], NewTrace, ECollisionChannel::ECC_Camera);
				//DrawDebugLine(GetWorld(), PreviousTraceLocs[i], NewTrace, FColor::Red, false, 5.f);

				if (bHit 
					&& HitResult.GetActor() != WeaponOwner
					&& CanAttackActor(HitResult.GetActor()))
				{
					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, HitResult.ImpactNormal * -1.f, HitResult, WeaponOwner->GetController(), WeaponOwner, DamageClass);
					//UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, WeaponOwner->GetController(), WeaponOwner, DamageClass);

					ICPPEnemyInterface* EnemyInterface = Cast<ICPPEnemyInterface>(HitResult.GetActor());
					ACPPPlayerCharacter* Player = Cast<ACPPPlayerCharacter>(GetWeaponOwner());

					if (EnemyInterface
						&& Player
						&& EnemyInterface->Execute_IsDead(HitResult.GetActor()))
					{
						Player->RemoveTarget(HitResult.GetActor());
					}
				}

				PreviousTraceLocs[i] = NewTrace;
			}
		}
	}
}

bool UCPPWeaponSkeletalMesh::CanAttackActor(class AActor* HitActor)
{
	if (HitActor
		&& AttackedActors.Find(HitActor) == -1
		&& ActorsToIgnore.Find(HitActor) == -1 //this one purely means that we should not attack anyone in the ignore actors array.
		&& HitActor != GetOwner())
	{
		AttackedActors.Add(HitActor);
		return true;
	}

	return false;
}

void UCPPWeaponSkeletalMesh::OnStartAttack()
{
	OnReceiveStartAttack();
}

void UCPPWeaponSkeletalMesh::OnStopAttack(float TimeSinceLastAttack)
{
	OnReceiveStopAttack(TimeSinceLastAttack);
}

void UCPPWeaponSkeletalMesh::OnToggleAttackTrace(bool bNewTrace)
{
	SetComponentTickEnabled(bNewTrace);
	bTraceAttack = bNewTrace;
	//only the assignment
	PreviousTraceLocs.Empty();

	if (!bNewTrace)
		AttackedActors.Empty();
}