// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CPPEnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCPPEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CPPCOURSE_API ICPPEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		bool IsDead();

	//To make sure only one enemy is engaging in attacking at any given time; the getter of the SetAttacking function 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		bool IsAttacking();

	virtual bool CanAttack() { return false; };

	//So that we have a function to set the attack on an enemy.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		void SetAttacking(bool bNewAttack);

	//Since we need the target data too for our utility blueprint
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		class AActor* GetTarget(bool& bHasValidTarget);

	//UI related, but we want health to be shown 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		void GetHealth(float& MaxHealth, float& CurrentHealth);

	//UI related, but we want health to be shown 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
		FString GetEnemyName();
};
