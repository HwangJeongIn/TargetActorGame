// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"
#include "TAHUDUserWidget.h"
#include "TAInventoryUserWidget.h"
#include "TAInteractionMenuUserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Common/CommonBase.h"
#include "Engine.h"


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

bool ATAPlayerController::getInventoryVisibility(void) const noexcept
{
	return (ESlateVisibility::Visible == _inventoryUserWidget->GetVisibility());
}

void ATAPlayerController::setInventoryVisibility(bool isVisible) const noexcept
{
	ESlateVisibility visibility = (true == isVisible) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	_inventoryUserWidget->SetVisibility(visibility);
}

void ATAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	FVector2D viewportSize(0, 0);
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	TA_LOG_DEV("viewport 1 : %.1f, %.1f", viewportSize.X, viewportSize.Y);

	// 값똑같이 나온다.
	//FVector2D viewportSize2(GEngine->GameViewport->Viewport->GetSizeXY());
	//TA_LOG_DEV("viewport 2 : %.1f, %.1f", viewportSize2.X, viewportSize2.Y);

	{
		_HUDUserWidget = CreateWidget<UTAHUDUserWidget>(this, _HUDUserWidgetClass);
		_HUDUserWidget->AddToViewport();
	}

	{
		_inventoryUserWidget = CreateWidget<UTAInventoryUserWidget>(this, _inventoryUserWidgetClass);
		UCanvasPanelSlot* inventoryCanvasPanelSlot = _HUDUserWidget->addChildWidgetToPanel(_inventoryUserWidget);
		if (nullptr == inventoryCanvasPanelSlot)
		{
			TA_ASSERT_DEV(false, "비정상");
		}
		else
		{
			//float tempY = viewportSize.Y / 2.0f;
			//float tempX = (viewportSize.X / 4.0f) / 2.0f;
			//float tempX2 = viewportSize.X / 4.0f;
			//TA_LOG_DEV("%.1f, %.1f, %.1f", tempY, tempX, tempX2);

			inventoryCanvasPanelSlot->SetAnchors(FAnchors(1.0f, 0.5f));
			inventoryCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			inventoryCanvasPanelSlot->SetPosition(FVector2D(-((viewportSize.X / 4.0f) / 2.0f), 0.0f)); // 화면의 1/4사이즈로 인벤토리 할 예정 피벗이 중앙이므로 2로 나눈값으로 위치를 잡는다.
			inventoryCanvasPanelSlot->SetSize(FVector2D(viewportSize.X / 4.0f, viewportSize.Y / 2.0f));

			//_inventoryUserWidget->AddToViewport();
			//_inventoryUserWidget->setInventorySlotCount(10);
			setInventoryVisibility(false);
			//_inventoryUserWidget->SetVisibility(ESlateVisibility::Hidden);
		}

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