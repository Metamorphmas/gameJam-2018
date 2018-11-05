// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Appendage.h"

/**
 * Class that stores the current legs
 */
class GAMEJAM2018_API Legs : Appendage
{
private:
	int type;
	class USkeletalMeshComponent *mesh;
	float speed;

public:
	Legs();
	Legs(int type);
	~Legs();
	void setLegsType(int type);
	float getSpeed();
};
