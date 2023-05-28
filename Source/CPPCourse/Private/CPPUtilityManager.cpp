// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPUtilityManager.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "CPPUtilityAITask.h"
#include "CPPEnemyInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "CPPCourse/CPPCourse.h"
#include "CPPEnemyCharacter.h"

// Sets default values for this component's properties
UCPPUtilityManager::UCPPUtilityManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCPPUtilityManager::BeginPlay()
{
	Super::BeginPlay();

	InitTasks();	
	ACPPEnemyCharacter* Character = Cast<ACPPEnemyCharacter>(GetOwner());
	if (Character)
		Character->OnCancelDueToDamage.AddDynamic(this, &UCPPUtilityManager::OnCancelDueToDamage);
}

void UCPPUtilityManager::InitTasks()
{
	if (TaskTemplates.Num() == 0)
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
		return;

	AAIController* Controller = Cast<AAIController>(Character->GetController());

	for (int32 i = 0; i < TaskTemplates.Num(); i++)
	{
		if (TaskTemplates.IsValidIndex(i)
			&& TaskTemplates[i]->IsValidLowLevelFast())
		{
			UCPPUtilityAITask* NewTask = NewObject<UCPPUtilityAITask>(this, TaskTemplates[i]);
			NewTask->Owner = Character;
			NewTask->Manager = this;
			NewTask->Controller = Controller;
			NewTask->Construct();

			Tasks.Add(NewTask);
			TaskScores.Add(0.f);

			UE_LOG(AnyLog, Warning, TEXT("Generated task with name %s from class %s"), *(NewTask->GetName()), *(TaskTemplates[i]->GetName()));
		}
	}
}

// Called every frame
void UCPPUtilityManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveTask)
		ActiveTask->Tick(DeltaTime);
}

void UCPPUtilityManager::EnableManager(float TaskTime)
{
	if (TaskTime > 0.f
		&& GetWorld()
		&& !GetWorld()->GetTimerManager().IsTimerActive(TaskTimer)
		&& Tasks.Num() != 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TaskTimer, this, &UCPPUtilityManager::ChooseTask, TaskTime, true);
		ChooseTask(); //take make sure we get an instant call to a task
	}
}

void UCPPUtilityManager::DisableManager()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TaskTimer);

		if (OnDisableManager.IsBound())
			OnDisableManager.Broadcast();

		ICPPEnemyInterface* EnemyInterface = Cast<ICPPEnemyInterface>(GetOwner());
		if (EnemyInterface)
			EnemyInterface->Execute_SetAttacking(GetOwner(), false);
	}
}

void UCPPUtilityManager::ChooseTask()
{
	UE_LOG(AnyLog, Warning, TEXT("Choose Task is Called"));

	if (ActiveTask
		&& !ActiveTask->TaskFinished())
		return;

	//score the tasks
	for (int32 i = 0; i < Tasks.Num(); i++)
	{
		TaskScores[i] = Tasks[i]->Score();
	}

	//set up the new tasks
	PreviousTask = ActiveTask;

	int32 TargetIndex; 
	float TargetValue;
	UKismetMathLibrary::MaxOfFloatArray(TaskScores, TargetIndex, TargetValue);

	UE_LOG(AnyLog, Warning, TEXT("Chosen Task Index is %d"), TargetIndex);

	ActiveTask = Tasks[TargetIndex];
	if (!ActiveTask)
	{
		UE_LOG(AnyLog, Error, TEXT("No active task!"));
		return;
	}

	//cancel previous task, if it's valid (first call will always fail, so we need this check)
	if(PreviousTask)
		PreviousTask->Stop();

	//process inactive tasks
	for (UCPPUtilityAITask* Task : Tasks)
	{
		if (Task != ActiveTask)
			Task->Inactive();
	}

	UE_LOG(AnyLog, Warning, TEXT("Trying to start New Task"));
	//finally start the new task
	ActiveTask->Start();
}

void UCPPUtilityManager::PauseTaskExecution(bool bNewPause)
{
	if (GetWorld())
	{
		if (bNewPause)
		{
			GetWorld()->GetTimerManager().PauseTimer(TaskTimer);
		}
		else
			GetWorld()->GetTimerManager().UnPauseTimer(TaskTimer);
	}
}

void UCPPUtilityManager::PauseTaskExecutionTimed(float PauseTime)
{
	if (GetWorld()
		&& PauseTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(PauseTimer, this, &UCPPUtilityManager::PauseTaskExecutionTimedFinished, PauseTime, false);
		GetWorld()->GetTimerManager().PauseTimer(TaskTimer);
	}
}

void UCPPUtilityManager::PauseTaskExecutionTimedFinished()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().UnPauseTimer(TaskTimer);
	}
}

void UCPPUtilityManager::OnCancelDueToDamage()
{
	ACPPEnemyCharacter* Character = Cast<ACPPEnemyCharacter>(GetOwner());
	if (Character)
	{
		Character->StopAnimMontage(); //if we keep this null, we can cancel whatever montage is currently playing. 
		Character->SetAttackTrace(false); //Make sure we turn off tracing. 
	}
}