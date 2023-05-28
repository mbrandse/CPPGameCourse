// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPProjectileManager.h"
#include "CPPEnemyProjectile.h"
#include "CPPCourse/CPPCourseCharacter.h"

// Sets default values
ACPPProjectileManager::ACPPProjectileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileIndex = 0;

}

// Called when the game starts or when spawned
void ACPPProjectileManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPPProjectileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPPProjectileManager::RainRandomProjectiles(ACPPCourseCharacter* ProjectileOwner, TSubclassOf<class ACPPEnemyProjectile> ProjectileTemplate, float Duration, float Interval, FVector Origin, float Range)
{
	if (ProjectileTemplate
		&& Range > 0.f
		&& Duration > 0.f
		&& Interval > 0.f
		&& GetWorld())
	{
		ProjectileIndex = 0;
		MaxProjectileIndex = (int32)FMath::Floor(Duration / Interval);
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimer, this, &ACPPProjectileManager::FireRandomProjectileFromSky, Interval, true);
		CurrentProjectileTemplate = ProjectileTemplate;
		CurrentProjectileOwner = ProjectileOwner;
		FireOrigin = Origin;
		FireRange = Range;
	}
}

void ACPPProjectileManager::RainFollowProjectiles(ACPPCourseCharacter* ProjectileOwner, TSubclassOf<class ACPPEnemyProjectile> ProjectileTemplate, float Duration, float Interval, float ZLocation, class AActor* TargetActor)
{
	if (ProjectileTemplate
		&& TargetActor
		&& Duration > 0.f
		&& Interval > 0.f
		&& GetWorld())
	{
		ProjectileIndex = 0;
		MaxProjectileIndex = (int32)FMath::Floor(Duration / Interval);
		GetWorld()->GetTimerManager().SetTimer(ProjectileTimer, this, &ACPPProjectileManager::FireFollowProjectileFromSky, Interval, true);
		CurrentProjectileTemplate = ProjectileTemplate;
		CurrentProjectileOwner = ProjectileOwner;
		CurrentTarget = TargetActor;
		FireRange = ZLocation;
	}
}

void ACPPProjectileManager::FireRandomProjectileFromSky()
{
	FActorSpawnParameters SpawnInfo;

	FVector TargetLoc, RandLoc;
	RandLoc.X = FMath::RandRange(-1.f, 1.f);
	RandLoc.Y = FMath::RandRange(-1.f, 1.f);
	float RandomRange = FMath::RandRange(0.f, FireRange);
	TargetLoc = RandLoc * RandomRange;
	TargetLoc += FireOrigin;

	FTransform NewTransform;
	NewTransform.SetTranslation(TargetLoc);
	NewTransform.SetRotation(FRotator(-90.f, 0.f, 0.f).Quaternion());
	NewTransform.SetScale3D(FVector(1.f));

	ACPPEnemyProjectile* NewProjectile = GetWorld()->SpawnActor<ACPPEnemyProjectile>(CurrentProjectileTemplate, NewTransform, SpawnInfo);
	NewProjectile->ProjectileOwner = CurrentProjectileOwner;

	ProjectileIndex++;
	if (ProjectileIndex > MaxProjectileIndex)
	{
		ProjectileIndex = 0;
		GetWorld()->GetTimerManager().ClearTimer(ProjectileTimer);
	}
}

void ACPPProjectileManager::FireFollowProjectileFromSky()
{
	FActorSpawnParameters SpawnInfo;

	FVector TargetLoc = CurrentTarget->GetActorLocation();
	TargetLoc.Z = FireRange;

	FTransform NewTransform;
	NewTransform.SetTranslation(TargetLoc);
	NewTransform.SetRotation(FRotator(-90.f, 0.f, 0.f).Quaternion());
	NewTransform.SetScale3D(FVector(1.f));
	
	ACPPEnemyProjectile* NewProjectile = GetWorld()->SpawnActor<ACPPEnemyProjectile>(CurrentProjectileTemplate, NewTransform, SpawnInfo);
	NewProjectile->ProjectileOwner = CurrentProjectileOwner;

	ProjectileIndex++;
	if (ProjectileIndex > MaxProjectileIndex)
	{
		ProjectileIndex = 0;
		GetWorld()->GetTimerManager().ClearTimer(ProjectileTimer);
	}
}