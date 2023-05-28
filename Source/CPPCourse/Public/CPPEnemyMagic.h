// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPEnemyCharacter.h"
#include "CPPEnemyMagic.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API ACPPEnemyMagic : public ACPPEnemyCharacter
{
	GENERATED_BODY()

protected:

	void BeginPlay();

	void Tick(float DeltaTime);

	FTimerHandle BrainTimerHandle;

public:

	ACPPEnemyMagic();

	void BrainUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float BrainUpdateInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		bool bAllowFriendlyFire;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float DetectionRange;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
		class ACPPPlayerCharacter* Target;

	TArray<class AActor*> IgnoreActors;

	FRotator DesiredRotation;

	UFUNCTION(BlueprintCallable, Category = "AI")
		void StartMagic();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI", meta = (DisplayName = "Start Magic"))
		void OnReceiveStartMagic();

	UFUNCTION(BlueprintCallable, Category = "AI")
		void FireMagic();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI", meta = (DisplayName = "Fire Magic"))
		void OnReceiveFireMagic();

	bool bIsFiring;

	UFUNCTION(BlueprintCallable, Category = "AI")
		void FinishMagic();
	
};
