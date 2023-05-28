// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPAnimNotify_AttackTrace.h"
#include "CPPCourse/CPPCourseCharacter.h"

void UCPPAnimNotify_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ACPPCourseCharacter* Character = Cast<ACPPCourseCharacter>(MeshComp->GetAttachmentRootActor());
	if (Character)
	{
		Character->SetAttackTrace(true);
		return;
	}
}

void UCPPAnimNotify_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ACPPCourseCharacter* Character = Cast<ACPPCourseCharacter>(MeshComp->GetAttachmentRootActor());
	if (Character)
	{
		Character->SetAttackTrace(false);
		return;
	}
}