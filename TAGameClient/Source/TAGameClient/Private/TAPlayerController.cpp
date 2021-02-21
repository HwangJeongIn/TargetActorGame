// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"


ATAPlayerController::ATAPlayerController()
{

}

void ATAPlayerController::PostInitializeComponents(void)
{
	Super::PostInitializeComponents();
}

void ATAPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ATAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ATAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}