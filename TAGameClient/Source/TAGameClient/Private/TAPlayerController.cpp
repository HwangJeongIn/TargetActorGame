// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"
#include "TAHUDUserWidget.h"
//#include "TAInventoryUserWidget.h"
#include "Common/CommonBase.h"

ATAPlayerController::ATAPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTAHUDUserWidget> GameHUD_C(TEXT("/Game/UI/GameHUD.GameHUD_C"));
	if (true == GameHUD_C.Succeeded())
	{
		_HUDUserWidgetClass = GameHUD_C.Class;
	}

	//static ConstructorHelpers::FClassFinder<UTAInventoryUserWidget> Inventory_C(TEXT("/Game/UI/Inventory.Inventory_C"));
	//if (true == Inventory_C.Succeeded())
	//{
	//	_inventoryUserWidgetClass = Inventory_C.Class;
	//}
}

void ATAPlayerController::PostInitializeComponents(void)
{
	Super::PostInitializeComponents();
}

void ATAPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

UTAHUDUserWidget* ATAPlayerController::getHUDUserWidget(void) const
{
	return _HUDUserWidget;
}

//UTAInventoryUserWidget* ATAPlayerController::getInventoryUserWidget(void) const
//{	
//	return _inventoryUserWidget;
//}

void ATAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	_HUDUserWidget = CreateWidget<UTAHUDUserWidget>(this, _HUDUserWidgetClass);
	_HUDUserWidget->AddToViewport();



	_HUDUserWidget->setInventorySlotCount(8);



	//_inventoryUserWidget = CreateWidget<UTAInventoryUserWidget>(this, _inventoryUserWidgetClass);
	//_inventoryUserWidget->AddToViewport();
}

void ATAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}