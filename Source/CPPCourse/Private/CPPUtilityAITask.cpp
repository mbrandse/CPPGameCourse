// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPUtilityAITask.h"
#include "CPPEnemyCharacter.h"

UCPPUtilityAITask::UCPPUtilityAITask()
{

}

bool UCPPUtilityAITask::CanAttack(class ACPPEnemyCharacter* Enemy)
{
	if (Enemy)
		return Enemy->CanAttack();

	return false;
}

void UCPPUtilityAITask::Construct()
{
	OnReceiveConstruct();
}

void UCPPUtilityAITask::Start()
{
	OnReceiveStart();
}

void UCPPUtilityAITask::Stop()
{
	OnReceiveStop();
}

void UCPPUtilityAITask::Inactive()
{
	OnReceiveInactive();
}

void UCPPUtilityAITask::Tick(float DeltaTime)
{
	OnReceiveTick(DeltaTime);
}

//float UCPPUtilityAITask::GetScore()
//{
//	SetScore();
//
//	return Score;
//}
//
//bool UCPPUtilityAITask::GetTaskFinished()
//{
//	SetTaskFinished();
//
//	return bTaskFinished;
//}