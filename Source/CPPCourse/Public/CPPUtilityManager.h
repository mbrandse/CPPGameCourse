// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPPUtilityManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableManagerSignature);

UCLASS( ClassGroup=(AI), meta=(BlueprintSpawnableComponent) )
class CPPCOURSE_API UCPPUtilityManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPPUtilityManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Manager related functions
	UFUNCTION()
		void InitTasks();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		void EnableManager(float TaskTime);

	UFUNCTION(BLueprintCallable, Category = "Manager")
		void DisableManager();

	UFUNCTION()
		void ChooseTask();

	UFUNCTION(BlueprintCallable, Category = "Manager")
		void PauseTaskExecution(bool bNewPause);

	UFUNCTION(BlueprintCallable, Category = "Manager")
		void PauseTaskExecutionTimed(float PauseTime);

	void PauseTaskExecutionTimedFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Manager")
		TArray<TSubclassOf<class UCPPUtilityAITask>> TaskTemplates;

	FTimerHandle TaskTimer;

	FTimerHandle PauseTimer;

	UPROPERTY()
		TArray<class UCPPUtilityAITask*> Tasks;

	UPROPERTY()
		TArray<float> TaskScores;

	UPROPERTY()
		class UCPPUtilityAITask* PreviousTask;

	UPROPERTY()
		class UCPPUtilityAITask* ActiveTask;
		
	UPROPERTY(BlueprintAssignable, Category = "Manager")
		FDisableManagerSignature OnDisableManager;

	UFUNCTION(BlueprintCallable, Category = "Manager")
		void OnCancelDueToDamage();
};
