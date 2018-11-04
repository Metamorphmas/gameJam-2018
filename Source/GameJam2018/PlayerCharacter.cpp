// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName APlayerCharacter::MoveForwardBinding("MoveForward");
const FName APlayerCharacter::MoveRightBinding("MoveRight");
const FName APlayerCharacter::FireForwardBinding("FireForward");
const FName APlayerCharacter::FireRightBinding("FireRight");

const FName APlayerCharacter::MovementModifierBinding("MovementModifier");

APlayerCharacter::APlayerCharacter()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CharacterMesh(TEXT("/Game/Meshes/Mech_Test1.Mech_Test1"));
	// Create the mesh component
	CharacterMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMesh"));
	RootComponent = CharacterMeshComponent;
	CharacterMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CharacterMeshComponent->SetStaticMesh(CharacterMesh.Object);

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-50.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	specialMovement = false;

	APlayerCharacter::movementModes[0].speed = 1000.0f;
	APlayerCharacter::movementModes[0].movementHandle = &APlayerCharacter::bipedMovement;
	APlayerCharacter::movementModes[0].movementReset = &APlayerCharacter::bipedReset;
	APlayerCharacter::movementModes[1].speed = 800.0f;
	APlayerCharacter::movementModes[1].movementHandle = &APlayerCharacter::quadripedMovement;
	APlayerCharacter::movementModes[1].movementReset = &APlayerCharacter::quadripedReset;
	APlayerCharacter::movementModes[2].speed = 800.0f;
	APlayerCharacter::movementModes[2].movementHandle = &APlayerCharacter::trackedMovement;
	APlayerCharacter::movementModes[2].movementReset = &APlayerCharacter::trackedReset;

	APlayerCharacter::movementMode = 1;

	// Movement
	MoveSpeed = movementModes[movementMode].speed;
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction(MovementModifierBinding, IE_Pressed, this, APlayerCharacter::movementModes[movementMode].movementHandle);
	PlayerInputComponent->BindAction(MovementModifierBinding, IE_Released, this, APlayerCharacter::movementModes[movementMode].movementReset);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}

	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	FireShot(FireDirection);
}

void APlayerCharacter::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				//World->SpawnActor<ATP_TwinStickProjectile>(SpawnLocation, FireRotation);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerCharacter::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void APlayerCharacter::ShotTimerExpired()
{
	bCanFire = true;
}

void APlayerCharacter::bipedMovement()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Biped"));
	MoveSpeed = 1400.0f;
}

void APlayerCharacter::bipedReset()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Biped ended"));
	MoveSpeed = 1000.0f;
}

void APlayerCharacter::quadripedMovement()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Quadriped"));
	if (specialMovement)
	{
		MoveSpeed = 800.0f;
	}
	else
	{
		MoveSpeed = 0.0f;
	}

	specialMovement = !specialMovement;
}

void APlayerCharacter::quadripedReset()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Quadriped ended"));
}

void APlayerCharacter::trackedMovement()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Tracked"));
}

void APlayerCharacter::trackedReset()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Tracked ended"));
}