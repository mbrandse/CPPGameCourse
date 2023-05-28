// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPPDoor.generated.h"

UCLASS()
class CPPCOURSE_API ACPPDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPPDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Door")
		bool AddDoorKey(class AActor* KeyActor);

	UFUNCTION(BlueprintCallable, Category = "Door")
		bool RemoveDoorKey(class AActor* KeyActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
		int32 NumberOfKeysToOpen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
		FName DoorID;

	int32 CurrentKeyCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
		bool bCanCloseAgain;

	TArray<class AActor*> KeyActors;

	bool bIsOpen;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
		void OpenDoor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
		void CloseDoor();

};
