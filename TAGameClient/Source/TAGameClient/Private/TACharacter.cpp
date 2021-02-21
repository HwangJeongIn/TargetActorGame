// Fill out your copyright notice in the Description page of Project Settings.


#include "TACharacter.h"
#include "TAAnimInstance.h"
#include "TAPawn.h"
#include "TAGameInstance.h"
#include "TAAIController.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATACharacter::ATACharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// generate components
	USkeletalMeshComponent* mesh = nullptr;
	UCapsuleComponent* capsule = nullptr;
	{
		capsule = GetCapsuleComponent();
		mesh = GetMesh();
	}

	capsule->SetCapsuleHalfHeight(88.f);
	capsule->SetCapsuleRadius(34.0f);
	// 콜리전 프리셋 지정
	capsule->SetCollisionProfileName(TEXT("TACharacter"));
	mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	//GetCharacterMovement()->JumpZVelocity = 500.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (true == SK_CARDBOARD.Succeeded())
	{
		mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance>WARRIOR_ANIM(TEXT("/Game/Animations/TAAnimationBlueprint.TAAnimationBlueprint_C"));
	if (true == WARRIOR_ANIM.Succeeded())
	{
		mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	AIControllerClass = ATAAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ATACharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATACharacter::setDeadAnimation(void) noexcept
{
	_animInstance->setDead(true);
	SetActorEnableCollision(false);
}
