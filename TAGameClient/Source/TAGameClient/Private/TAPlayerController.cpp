// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"
#include "TAHUDUserWidget.h"
#include "TAInventoryUserWidget.h"
#include "TAInteractionMenuUserWidget.h"
#include "Common/CommonBase.h"


ATAPlayerController::ATAPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTAHUDUserWidget> GameHUD_C(TEXT("/Game/UI/GameHUD.GameHUD_C"));
	TA_ASSERT_DEV(true == GameHUD_C.Succeeded(), "비정상");
	if (true == GameHUD_C.Succeeded())
	{
		_HUDUserWidgetClass = GameHUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UTAInventoryUserWidget> Inventory_C(TEXT("/Game/UI/Inventory.Inventory_C"));
	TA_ASSERT_DEV(true == Inventory_C.Succeeded(), "비정상");
	if (true == Inventory_C.Succeeded())
	{
		_inventoryUserWidgetClass = Inventory_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UTAInteractionMenuUserWidget> InteractionMenu_C(TEXT("/Game/UI/InteractionMenu.InteractionMenu_C"));
	TA_ASSERT_DEV(true == InteractionMenu_C.Succeeded(), "비정상");
	if (true == InteractionMenu_C.Succeeded())
	{
		_interactionMenuUserWidgetClass = InteractionMenu_C.Class;
	}
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

UTAInventoryUserWidget* ATAPlayerController::getInventoryUserWidget(void) const
{	
	return _inventoryUserWidget;
}

void ATAPlayerController::openInventory(void) const noexcept
{
	_interactionMenuUserWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ATAPlayerController::closeInventory(void) const noexcept
{
	_inventoryUserWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ATAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	{
		_HUDUserWidget = CreateWidget<UTAHUDUserWidget>(this, _HUDUserWidgetClass);
		_HUDUserWidget->AddToViewport();
	}

	{
		_inventoryUserWidget = CreateWidget<UTAInventoryUserWidget>(this, _inventoryUserWidgetClass);
		_inventoryUserWidget->AddToViewport();
		_inventoryUserWidget->setInventorySlotCount(10);
		_inventoryUserWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	{
		_interactionMenuUserWidget = CreateWidget<UTAInteractionMenuUserWidget>(this, _interactionMenuUserWidgetClass);
		_interactionMenuUserWidget->AddToViewport();
		_interactionMenuUserWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void ATAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}