// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPPProjectileManager.generated.h"

UCLASS()
class CPPCOURSE_API ACPPProjectileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPPProjectileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void RainRandomProjectiles(class ACPPCourseCharacter* ProjectileOwner, TSubclassOf<class ACPPEnemyProjectile> ProjectileTemplate, float Duration, float Interval, FVector Origin, float Range);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void RainFollowProjectiles(class ACPPCourseCharacter* ProjectileOwner, TSubclassOf<class ACPPEnemyProjectile> ProjectileTemplate, float Duration, float Interval, float ZLocation, class AActor* TargetActor);

	int32 ProjectileIndex;
	int32 MaxProjectileIndex;

	FVector FireOrigin;
	float FireRange;
	class AActor* CurrentTarget;
	TSubclassOf<class ACPPEnemyProjectile> CurrentProjectileTemplate;
	class ACPPCourseCharacter* CurrentProjectileOwner;

	FTimerHandle ProjectileTimer;

	void FireRandomProjectileFromSky();

	void FireFollowProjectileFromSky();
};
