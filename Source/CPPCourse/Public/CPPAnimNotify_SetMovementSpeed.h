// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CPPAnimNotify_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Movement Speed"))
class CPPCOURSE_API UCPPAnimNotify_SetMovementSpeed : public UAnimNotify
{
	GENERATED_BODY()

public:

	UCPPAnimNotify_SetMovementSpeed(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bResetToDefaultSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "!bResetToDefaultSpeed"))
		float NewMovementSpeed;
	
};
