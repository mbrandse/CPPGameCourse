// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPPSwitch.generated.h"

UCLASS()
class CPPCOURSE_API ACPPSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPPSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Switch")
		class ACPPDoor* TargetDoor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Switch")
		FName TargetDoorID;

	UFUNCTION(BlueprintCallable, Category = "Switch")
		bool ActivateSwitch();

	UFUNCTION(BlueprintCallable, Category = "Switch")
		bool DeactivateSwitch();
};
