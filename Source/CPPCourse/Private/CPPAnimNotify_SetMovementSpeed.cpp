// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPAnimNotify_SetMovementSpeed.h"
#include "CPPCourse/CPPCourseCharacter.h"
#include "CPPPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UCPPAnimNotify_SetMovementSpeed::UCPPAnimNotify_SetMovementSpeed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NewMovementSpeed = 100.0f;
}

void UCPPAnimNotify_SetMovementSpeed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//first attempt to get player. 
	ACPPCourseCharacter* Character = Cast<ACPPCourseCharacter>(MeshComp->GetAttachmentRootActor());
	if (Character)
	{
		ACPPPlayerCharacter* Player = Cast<ACPPPlayerCharacter>(Character);

		if (bResetToDefaultSpeed)
		{
			ACharacter* DefaultCharacter = Cast<ACharacter>(Character->GetClass()->GetDefaultObject(true));
			Character->GetCharacterMovement()->MaxWalkSpeed = DefaultCharacter->GetCharacterMovement()->MaxWalkSpeed;

			if (Player)
			{
				ACPPPlayerCharacter* DefaultPlayer = Cast<ACPPPlayerCharacter>(Player->GetClass()->GetDefaultObject(true));
				Player->MaxStrafeSpeed = DefaultPlayer->MaxStrafeSpeed;
			}
		}
		else
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
			if (NewMovementSpeed == 0.f)
				Character->GetCharacterMovement()->StopActiveMovement();

			if (Player)
				Player->MaxStrafeSpeed = NewMovementSpeed;
		}
		

		return;
	}
}