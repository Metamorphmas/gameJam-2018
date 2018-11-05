// Fill out your copyright notice in the Description page of Project Settings.

#include "Appendage.h"

int Appendage::getMaxHealth()
{
	return Appendage::maxHealth;
}

int Appendage::getCurrentHealth()
{
	return Appendage::currentHealth;
}

float Appendage::getArmor()
{
	return Appendage::armor;
}

bool Appendage::damage(int damage)
{
	Appendage::currentHealth -= int(Appendage::armor * damage);
	return Appendage::currentHealth <= 0;
}