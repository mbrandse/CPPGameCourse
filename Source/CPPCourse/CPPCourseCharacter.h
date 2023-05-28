// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPPCourseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartAttackSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStopAttackSignature, float, TimeSinceAttackStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToggleWeaponTraceSignature, bool, bNewTrace);

UENUM(BlueprintType)
enum class EDamageWeight : uint8
{
	PDA_None UMETA(DisplayName = "No Damage"), //default value, used to make sure we have a null state as well
	PDA_Light UMETA(DisplayName = "Light Damage"),
	PDA_Heavy UMETA(DisplayName = "Heavy Damage")
};

UCLASS(config=Game)
class ACPPCourseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACPPCourseCharacter();

public:

	//-------------------------------------------------------------------------------------------------------------
	// ATTACK
	//-------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "Attack")
		FStartAttackSignature OnStartAttack;

	UPROPERTY(BlueprintAssignable, Category = "Attack")
		FStopAttackSignature OnStopAttack;

	UPROPERTY(BlueprintAssignable, Category = "Attack")
		FToggleWeaponTraceSignature OnToggleAttackTrace;

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetAttackTrace(bool bNewTrace);

	UPROPERTY()
		float TimeSinceAttackStarted;

	void StartAttack();

	void StopAttack();

	UFUNCTION(BlueprintCallable, Category = "Death")
		virtual void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Death", meta = (DisplayName = "On Death"))
		void ReceiveOnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		float MaxHealth;

	UPROPERTY()
		float CurrentHealth;
};

