// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CPPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API UCPPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Transform")
		static FTransform RotateActorAroundTarget(class AActor* SourceActor, class AActor* TargetActor, float ForwardSpeed, float SidewaysSpeed, FVector RotationVector = FVector(0.f, 0.f, 1.f));
	
};
