// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPFunctionLibrary.h"

FTransform UCPPFunctionLibrary::RotateActorAroundTarget(AActor* SourceActor, AActor* TargetActor, float ForwardSpeed, float SidewaysSpeed, FVector RotationVector)
{
	FTransform TargetTransform = FTransform::Identity; //get an empty transform

	if (SourceActor 
		&& TargetActor)
	{
		//Setup our required variables
		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector SourceLocation = SourceActor->GetActorLocation();
		TargetLocation.Z = SourceLocation.Z;
		FVector TargetDimension, RotateValue;
		FRotator TargetRotation;
		float TargetAngle;
		float TargetDistance = FVector::Dist2D(TargetLocation, SourceLocation); //initialize target distance.

		//Setup dimensions
		float TargetDistanceIncrease = TargetDistance + ForwardSpeed; //Since we use the target distance increase elsewhere, completely calculate it here
		TargetDimension = FVector(TargetDistanceIncrease, 0.f, 0.f); //we want the x axis here as that corresponds with the axis that is getting offsetted

		//We get the lookat rotation, so we can use the yaw for further calculations
		TargetRotation = FRotationMatrix::MakeFromX(TargetLocation - SourceLocation).Rotator(); //this gets the heading towards the target

		//Calculate the necessary increase in angle based on input.
		TargetAngle = (TargetRotation.Yaw - 180.f) + SidewaysSpeed; //we need to take off 180, since we need to swap perspectives (not player -> target, but the other way around)
		RotateValue = TargetDimension.RotateAngleAxis(TargetAngle, RotationVector); //This rotates TargetDimension around the TargetingVector using TargetAngle degrees
		TargetLocation += RotateValue;

		FQuat QuatRot = FQuat(TargetRotation);

		TargetTransform.SetTranslation(TargetLocation);
		TargetTransform.SetRotation(QuatRot);
	}

	return TargetTransform;
}

