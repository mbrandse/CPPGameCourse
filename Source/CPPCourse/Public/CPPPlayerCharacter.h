// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPCourse/CPPCourseCharacter.h"
#include "CPPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API ACPPPlayerCharacter : public ACPPCourseCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Target camera */
	UPROPERTY()
		class ACameraActor* TargetCamera;

public:

	ACPPPlayerCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		FVector TargetCameraLocation;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	void BeginPlay();

	void Tick(float DeltaTime);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	void TurnAtRateMouse(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void LookUpAtRateMouse(float Rate);

	void StartJump();
	void StopJump();

public:

	//-------------------------------------------------------------------------------------------------------------
	//Camera Getters
	//-------------------------------------------------------------------------------------------------------------

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//-------------------------------------------------------------------------------------------------------------
	//Targeting System
	//-------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		TArray<class AActor*> TargetingActors;

	//Despite using arrays, the reason we take a targeting actor here is so that we don't get weird switches when our close target array suddenly changes. 
	UPROPERTY()
		AActor* TargetingActor;

	//we want this data for UI purposes only
	UPROPERTY()
		AActor* OldTargetingActor;

	//The strafe speed during targeting
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Targeting")
		float MaxStrafeSpeed;

	//The strafe speed during targeting
	UPROPERTY()
		FVector StrafeVelocity;

	UFUNCTION(BlueprintPure, Category = "Targeting")
		FVector GetStrafeVelocity() { return StrafeVelocity; };

	//The max distance of objects we should be able to target
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Targeting")
		float MaxTargetingDistance;

	UPROPERTY()
		bool bIsTargeting;

	UPROPERTY()
		float MoveRightValue;

	UFUNCTION(BlueprintPure, Category = "Targeting", meta = (DisplayName = "IsTargeting"))
		bool GetIsTargeting() { return bIsTargeting; };

	//The current actor being targeted
	UPROPERTY()
		int32 TargetingIndex;

	//This is the up vector of the vector we want to rotate
	UPROPERTY()
		FVector TargetingVector;

	UPROPERTY()
		float TargetAngleIncrease;

	UPROPERTY()
		float SidewaysStrafeSpeed;

	UPROPERTY()
		float TargetDistanceIncrease;

	void GetAllTargetingActors();
	TArray<AActor*> GetAllTargetingActorsWithinRange();

	//Have a function where we can control the camera too; since targeting will use the inputs that are normally used for targeting
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		void GetNextTarget();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		void GetPreviousTarget();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		void ToggleTargeting();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		void RemoveTarget(class AActor* TargetToRemove);

	//-------------------------------------------------------------------------------------------------------------
	// GENERAL MOVEMENT
	//-------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		float bCanMove;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
		void SetCanMove(bool bNewCanMove) { bCanMove = bNewCanMove; };


	//-------------------------------------------------------------------------------------------------------------
	// DAMAGE & HEALTH
	//-------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakePointDamage(class AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector HitFromDirection, const class UDamageType* DamageType, class AActor* DamageCauser);
	
	void Landed(const FHitResult& Hit) override;

	float RequestedDamageRecoveryTime;
	
	UFUNCTION(BlueprintCallable, Category = "Damage")
		void OnFinishDamageCooldown();

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		EDamageWeight DamageAnimationState;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
		bool bIsDead;

	void OnDeath() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
		void OnDamage(float MaxPlayerHealth, float CurrentPlayerHealth);
};
