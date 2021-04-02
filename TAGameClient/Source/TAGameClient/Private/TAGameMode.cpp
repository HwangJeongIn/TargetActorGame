// Fill out your copyright notice in the Description page of Project Settings.


#include "TAGameMode.h"
#include "Common/CommonBase.h"
#include "TAPlayer.h"
#include "TAPlayerController.h"
//#include "TAHudUserWidget.h"


ATAGameMode::ATAGameMode()
{
	TA_LOG_DEV("CurrentMode : ATAGameMode");
	DefaultPawnClass = ATAPlayer::StaticClass();
	PlayerControllerClass = ATAPlayerController::StaticClass();

	//static ConstructorHelpers::FClassFinder<UTAHudUserWidget> GameHud_C(TEXT("/Game/_UI/GameHud.GameHud_C"));
	//TA_ASSERT_DEV(true == GameHud_C.Succeeded(), "비정상");
	//if (true == GameHud_C.Succeeded())
	//{
	//	HUDClass = GameHud_C.Class;
	//}
}