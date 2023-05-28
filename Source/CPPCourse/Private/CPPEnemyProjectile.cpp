// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPEnemyProjectile.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "CPPCourse/CPPCourseCharacter.h"

// Sets default values
ACPPEnemyProjectile::ACPPEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpeed = 1000.f;
	ExplosionRadius = 150.f;
	Damage = 1.f;
	ScanRadius = 32.f;
}

// Called every frame
void ACPPEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ProjectileOwner
		&& GetWorld()
		&& ExplosionTemplate)
	{
		FVector NewLoc = GetActorLocation();
		NewLoc += (GetActorForwardVector() * ProjectileSpeed * DeltaTime);

		SetActorLocation(NewLoc);
		

		FHitResult FirstHit;
		bool bHit = GetWorld()->SweepSingleByChannel(FirstHit, NewLoc, NewLoc + FVector(0.f, 0.f, 5.f), FQuat::Identity, ECollisionChannel::ECC_Camera, FCollisionShape::MakeSphere(ScanRadius));

		if (bHit
			&& FirstHit.GetActor() != this
			&& FirstHit.GetActor() != ProjectileOwner)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionTemplate, NewLoc);
			if (CameraShakeTemplate)
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeTemplate, NewLoc, 0.f, ExplosionRadius * 10.f, 1.f, false);

			TArray<FHitResult> DamageHits;
			GetWorld()->SweepMultiByChannel(DamageHits, NewLoc, NewLoc + FVector(0.f, 0.f, 5.f), FQuat::Identity, ECollisionChannel::ECC_Camera, FCollisionShape::MakeSphere(ExplosionRadius));
			if (DamageHits.Num() != 0)
			{
				for (FHitResult Hit : DamageHits)
				{
					if(Hit.GetActor()
						&& Hit.GetActor() != this
						&& CanHitActor(Hit.GetActor()))
						UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.ImpactNormal*-1.f, Hit, ProjectileOwner->GetController(), ProjectileOwner, DamageClass);
				}
			}

			//Destroy the component in the end
			Destroy();
		}
	}
}

bool ACPPEnemyProjectile::CanHitActor(AActor* ActorToHit)
{
	if (ActorToHit)
	{
		int32 Index = HitActors.Find(ActorToHit);
		if (Index == -1)
		{
			HitActors.Add(ActorToHit);
			return true;
		}
	}

	return false;
}