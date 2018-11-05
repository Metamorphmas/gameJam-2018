// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Abstract class that contains health and armor information
 */
class GAMEJAM2018_API Appendage
{
private:
	int maxHealth;
	int currentHealth;
	float armor;

public:
	int getMaxHealth();
	int getCurrentHealth();
	float getArmor();
	bool damage(int damage);
};
