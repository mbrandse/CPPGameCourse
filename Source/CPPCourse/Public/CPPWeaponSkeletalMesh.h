// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "CPPWeaponSkeletalMesh.generated.h"

/**
 * 
 */
UCLASS()
class CPPCOURSE_API UCPPWeaponSkeletalMesh : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:

	UCPPWeaponSkeletalMesh(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
		class ACPPCourseCharacter* WeaponOwner;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attack")
		class ACPPCourseCharacter* GetWeaponOwner() { return WeaponOwner;};

	//Basic attack binds
	//THIS NEEDS TO BE A UFUNCTION, OR OUR BINDS WILL NEVER WORK
	UFUNCTION()
		void OnStartAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack", meta = (DisplayName = "On Start Attack"))
		void OnReceiveStartAttack();

	UFUNCTION()
		void OnStopAttack(float TimeSinceLastAttack);

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack", meta = (DisplayName = "On Stop Attack"))
		void OnReceiveStopAttack(float TimeSinceLastAttack);

	UFUNCTION()
		void OnToggleAttackTrace(bool bNewTrace);

	bool CanAttackActor(class AActor* HitActor);

	UPROPERTY()
		bool bTraceAttack;

	UPROPERTY()
		TArray<class AActor*> AttackedActors;

	UPROPERTY()
		TArray<class AActor*> ActorsToIgnore;

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void SetActorsToIgnore(TArray<class AActor*> Actors) { ActorsToIgnore = Actors; };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
		FName TraceStartSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
		FName TraceEndSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
		float Damage;

	//For assignment
	UPROPERTY()
		TArray<FVector> PreviousTraceLocs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
		TSubclassOf<class UDamageType> DamageClass;
};
