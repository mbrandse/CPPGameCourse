// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPCourse/CPPCourseCharacter.h"
#include "CPPEnemyInterface.h"
#include "CPPEnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ACPPEnemyCharacter*, Enemy);

UCLASS()
class CPPCOURSE_API ACPPEnemyCharacter : public ACPPCourseCharacter, public ICPPEnemyInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCPPUtilityManager* UtilityManager;

public:
	// Sets default values for this character's properties
	ACPPEnemyCharacter();

	/** Returns CameraBoom subobject **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
		class UCPPUtilityManager* GetUtilityManager() const { return UtilityManager; }

	//DAMAGE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<class UCPPUIEnemyHealth> EnemyHealthUITemplate;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		class UCPPUIEnemyHealth* EnemyHealthUI;

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage", meta = (DisplayName = "On Take Damage (Custom)"))
		void ReceiveTakeDamage(float Damage, TSubclassOf<class UDamageType> DamageTypeClass, AController* EventInstigator, AActor* DamageCauser);

	void OnDeath() override;

	virtual bool IsDead_Implementation();

	virtual void GetHealth_Implementation(float& InterfaceMaxHealth, float& InterfaceCurrentHealth);

	FTimerHandle HideUITimer;

	void HideHealthUI();

	UPROPERTY(BlueprintAssignable, Category = "Damage")
		FOnDamageSignature OnCancelDueToDamage;

	UPROPERTY(BlueprintAssignable, Category = "Damage")
		FOnDeathSignature OnEnemyDeath;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//INTERFACE
	virtual bool CanAttack() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
		void PauseAIExecution(bool bNewPause);
};
