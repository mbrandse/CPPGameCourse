// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CPPUtilityAITask.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CPPCOURSE_API UCPPUtilityAITask : public UObject
{
	GENERATED_BODY()

public:

	UCPPUtilityAITask();

	UFUNCTION(BlueprintPure, Category = "Information")
		bool CanAttack(class ACPPEnemyCharacter* Enemy);

	void Construct();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "Construct"))
		void OnReceiveConstruct();

	void Start();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "Start"))
		void OnReceiveStart();

	void Stop();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "Stop"))
		void OnReceiveStop();

	void Inactive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "Inactive"))
		void OnReceiveInactive();
	
	void Tick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Task", meta = (DisplayName = "Tick"))
		void OnReceiveTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Task")
		float Score();

	UFUNCTION(BlueprintImplementableEvent, Category = "Task")
		bool TaskFinished();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task")
		class AActor* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task")
		class UCPPUtilityManager* Manager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task")
		class AAIController* Controller;
};
