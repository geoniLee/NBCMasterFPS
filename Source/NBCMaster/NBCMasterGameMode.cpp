// Copyright Epic Games, Inc. All Rights Reserved.

#include "NBCMasterGameMode.h"
#include "NBCMasterCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANBCMasterGameMode::ANBCMasterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
