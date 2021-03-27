// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "EngineMinimal.h"


// Sets default values
ATAPawn::ATAPawn()
{
	_movementComponent = nullptr;
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// generate components

	_capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	_movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	_springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINTARM"));
	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));


	// capsule > mesh > springArm > camera
	RootComponent = _capsule;
	_mesh->SetupAttachment(_capsule);
	_springArm->SetupAttachment(_capsule);
	_camera->SetupAttachment(_springArm);

	_capsule->SetCapsuleHalfHeight(88.f);
	_capsule->SetCapsuleRadius(34.0f);
	_mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	_springArm->TargetArmLength = 400.f;
	_springArm->SetRelativeRotation(FRotator(-15.0f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_CARDBOARD(TEXT("/Game/_Dev/_Characters/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (true == SK_CARDBOARD.Succeeded())
	{
		_mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
	}
	
	
	//_mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//static ConstructorHelpers::FClassFinder<UAnimInstance>WARRIOR_ANIM(TEXT("/Game/Animations/WarriorAnimationBlueprint.WarriorAnimationBlueprint_C"));
	//if (true == WARRIOR_ANIM.Succeeded())
	//{
	//	// 블루프린트로 스켈레탈 메시의 애니메이션 인스턴스를 세팅한다.
	//	_mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	//}
}

// Called when the game starts or when spawned
void ATAPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATAPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATAPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 함수 바인딩
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &ATAPawn::upDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ATAPawn::leftRight);

	_movementComponent = nullptr;
	_movementComponent = GetMovementComponent();
}

void ATAPawn::PostInitializeComponents(void)
{
	Super::PostInitializeComponents();
}

void ATAPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ATAPawn::upDown(float newAxisValue) noexcept
{
	FVector direction = GetActorForwardVector();
	if (_movementComponent)
	{
		_movementComponent->AddInputVector(direction * newAxisValue, false);
	}
	else
	{
		Internal_AddMovementInput(direction * newAxisValue, false);
	}
}

void ATAPawn::leftRight(float newAxisValue) noexcept
{
	FVector direction = GetActorRightVector();
	if (_movementComponent)
	{
		_movementComponent->AddInputVector(direction * newAxisValue, false);
	}
	else
	{
		Internal_AddMovementInput(direction * newAxisValue, false);
	}
}
