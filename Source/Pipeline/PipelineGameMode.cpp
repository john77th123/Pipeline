// Copyright Epic Games, Inc. All Rights Reserved.

#include "PipelineGameMode.h"
#include "PipelineCharacter.h"
#include "UObject/ConstructorHelpers.h"

APipelineGameMode::APipelineGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
