// Fill out your copyright notice in the Description page of Project Settings.

#include "GameJam2018GameModeBase.h"
#include "PlayerCharacter.h"

AGameJam2018GameModeBase::AGameJam2018GameModeBase()
{
	// set default pawn class to our character class
	DefaultPawnClass = APlayerCharacter::StaticClass();
}


