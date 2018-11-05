// Fill out your copyright notice in the Description page of Project Settings.

#include "Legs.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/Object.h"

Legs::Legs()
{
	setLegsType(1);
}

Legs::Legs(int type)
{
	setLegsType(type);
}

Legs::~Legs()
{

}

void Legs::setLegsType(int type)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BipedMesh(TEXT("/Game/Meshes/MechParts/Mesh_BipedLegs/Mech_Legs_Biped1_RIGTEST2.Mech_Legs_Biped1_RIGTEST2"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuadMesh(TEXT("/Game/Meshes/MechParts/Mesh_QuadLegs/Mech_QuadLegs1.Mech_QuadLegs1"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TreadMesh(TEXT("/Game/Meshes/MechParts/Mesh_TreadLegs/Mech_TreadLegs1.Mech_TreadLegs1"));

	Legs::type = type;
	switch (type)
	{
	case 0:
		Legs::mesh->SetSkeletalMesh(BipedMesh.Object);
		Legs::speed = 1000.0f;
		break;
	case 1:
		Legs::mesh->SetSkeletalMesh(QuadMesh.Object);
		Legs::speed = 800.0f;
		break;
	case 2:
		Legs::mesh->SetSkeletalMesh(TreadMesh.Object);
		Legs::speed = 800.0f;
		break;
	}
}

float Legs::getSpeed()
{
	return Legs::speed;
}