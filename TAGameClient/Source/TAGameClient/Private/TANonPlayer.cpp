// Fill out your copyright notice in the Description page of Project Settings.


#include "TANonPlayer.h"
#include "TAAIController.h"



ATANonPlayer::ATANonPlayer()
{

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	bUseControllerRotationYaw = false;
	characterMovement->bOrientRotationToMovement = true; // 가는 방향으로 자동회전
	characterMovement->bUseControllerDesiredRotation = false;
	characterMovement->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
}

void ATANonPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//return;

	// AController* controller = GetController();
	// 
	// static float time = 0.0f;
	// time += DeltaTime;
	// if (time > 3.0f)
	// {
	// 	time = 0.0f;
	// 	AAIController* test1 = Cast<AAIController>(controller);
	// 	APlayerController* test2 = Cast<APlayerController>(controller);
	// 
	// 	if (nullptr != test1)
	// 	{
	// 		TA_LOG_DEV("AIController1");
	// 		test1->MoveToLocation(GetActorLocation() + FVector(600, 0, 0), 1.0f);
	// 
	// 		return;
	// 	}
	// 	
	// 	if (nullptr != test2)
	// 	{
	// 		TA_LOG_DEV("PlayerController");
	// 	}
	// }
}