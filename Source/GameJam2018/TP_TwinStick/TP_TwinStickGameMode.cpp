// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TP_TwinStickGameMode.h"
#include "TP_TwinStickPawn.h"

ATP_TwinStickGameMode::ATP_TwinStickGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ATP_TwinStickPawn::StaticClass();
}

