// Fill out your copyright notice in the Description page of Project Settings.


#include "TAGameMode.h"
#include "TAPlayer.h"
#include "TAPawn.h"
#include "TAPlayerController.h"


ATAGameMode::ATAGameMode()
{
	DefaultPawnClass = ATAPlayer::StaticClass();
	PlayerControllerClass = ATAPlayerController::StaticClass();
}