// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPDoor.h"

// Sets default values
ACPPDoor::ACPPDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsOpen = false;
	bCanCloseAgain = false;
	NumberOfKeysToOpen = 1;
	DoorID = TEXT("Door01");
}

// Called when the game starts or when spawned
void ACPPDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPPDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACPPDoor::AddDoorKey(AActor* KeyActor)
{
	if (KeyActor
		&& !bIsOpen)
	{
		uint32 Index = KeyActors.Find(KeyActor);
		if (Index == -1)
		{
			KeyActors.Add(KeyActor);
			CurrentKeyCount++;
			if (CurrentKeyCount == NumberOfKeysToOpen)
			{
				OpenDoor();
				bIsOpen = true;
				return true;
			}
		}
	}

	return false;
}

bool ACPPDoor::RemoveDoorKey(AActor* KeyActor)
{
	if (KeyActor)
	{
		uint32 Index = KeyActors.Find(KeyActor);
		if (Index != -1)
		{
			KeyActors.Remove(KeyActor);
			CurrentKeyCount--;
			if (CurrentKeyCount < 0)
				CurrentKeyCount = 0; //this should not be necessary, but let's do it anyway

			if(CurrentKeyCount != NumberOfKeysToOpen
				&& bIsOpen
				&& bCanCloseAgain)
			{
				CloseDoor();
				bIsOpen = false;
				return true;
			}
		}
	}

	return false;
}