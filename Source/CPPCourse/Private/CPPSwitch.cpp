// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPSwitch.h"
#include "Kismet/GameplayStatics.h"
#include "CPPDoor.h"

// Sets default values
ACPPSwitch::ACPPSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPPSwitch::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()
		&& !TargetDoor)
	{
		TArray<AActor*> DoorActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPPDoor::StaticClass(), DoorActors);
		if (DoorActors.Num() != 0)
		{
			for (AActor* DoorActor : DoorActors)
			{
				ACPPDoor* Door = Cast<ACPPDoor>(DoorActor);
				if(Door
					&& Door->DoorID == TargetDoorID)
					TargetDoor = Door;
			}
		}
	}
}

// Called every frame
void ACPPSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACPPSwitch::ActivateSwitch()
{
	if (TargetDoor)
	{
		return TargetDoor->AddDoorKey(this);
	}

	return false;
}

bool ACPPSwitch::DeactivateSwitch()
{
	if (TargetDoor)
	{
		return TargetDoor->RemoveDoorKey(this);
	}

	return false;
}