// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "CPPCourse/CPPCourseCharacter.h"
#include "CPPDamageBase.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API UCPPDamageBase : public UDamageType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		EDamageWeight DamageWeight;
};
