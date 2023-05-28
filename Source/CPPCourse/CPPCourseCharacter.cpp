// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPPCourseCharacter.h"
#include "CPPCourse.h"
#include "CPPWeaponSkeletalMesh.h"
#include "Misc/DateTime.h"

//////////////////////////////////////////////////////////////////////////
// ACPPCourseCharacter

ACPPCourseCharacter::ACPPCourseCharacter()
{
	
}

void ACPPCourseCharacter::StartAttack()
{
	if(OnStartAttack.IsBound())
	{
		FDateTime CurrentTime = FDateTime::Now();

		OnStartAttack.Broadcast();
		float CurrentMilliSec = (float)CurrentTime.GetMillisecond() / 1000.f;
		TimeSinceAttackStarted = (float)CurrentTime.GetSecond();
		TimeSinceAttackStarted += CurrentMilliSec;
	}
}

void ACPPCourseCharacter::StopAttack()
{
	if (OnStopAttack.IsBound())
	{ 
		FDateTime CurrentTime = FDateTime::Now();

		float CurrentMilliSec = (float)CurrentTime.GetMillisecond() / 1000.f;
		float CurrentSec = (float)CurrentTime.GetSecond();
		CurrentSec += CurrentMilliSec;

		OnStopAttack.Broadcast(CurrentSec - TimeSinceAttackStarted);
	}
}

void ACPPCourseCharacter::SetAttackTrace(bool bNewTrace)
{
	if (OnToggleAttackTrace.IsBound())
	{
		OnToggleAttackTrace.Broadcast(bNewTrace);
	}
}

void ACPPCourseCharacter::OnDeath()
{
	ReceiveOnDeath();
}