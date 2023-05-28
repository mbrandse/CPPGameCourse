// Copyright Epic Games, Inc. All Rights Reserved.

#include "CPPCourseGameMode.h"
#include "CPPCourseCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACPPCourseGameMode::ACPPCourseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
