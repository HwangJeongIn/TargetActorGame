// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"
#include "TACharacter.h"
#include "TAHudUserWidget.h"
#include "TAInventoryUserWidget.h"
#include "TAInteractionMenuUserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Common/CommonBase.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Client/ClientActor.h"
#include "Client/ClientInventoryActorComponent.h"
#include "Client/ClientInventoryActorSystem.h"
#include "Engine.h"


ATAPlayerController::ATAPlayerController()
{
	static ConstructorHelpers::FClassFinder<UTAHudUserWidget> GameHud_C(TEXT("/Game/UI/GameHud.GameHud_C"));
	TA_ASSERT_DEV(true == GameHud_C.Succeeded(), "비정상");
	if (true == GameHud_C.Succeeded())
	{
		_hudUserWidgetClass = GameHud_C.Class;
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

	_hoveredUi = UiBoundaryType::None;
}

void ATAPlayerController::PostInitializeComponents(void)
{
	Super::PostInitializeComponents();
}

void ATAPlayerController::OnPossess(APawn* aPawn)
{
	TA_LOG_DEV("player controller OnPossess");
	Super::OnPossess(aPawn);
}

UTAHudUserWidget* ATAPlayerController::getHudUserWidget(void) const
{
	return _hudUserWidget;
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

void ATAPlayerController::onHoveredUi(UTAChunkUserWidget* input) noexcept
{
	const UiBoundaryType boundaryType = input->getBoundaryType();
	if (boundaryType <= UiBoundaryType::Hud)
	{
		return;
	}

	_hoveredUi = boundaryType;
	TA_LOG_DEV("hovedUi = %d", static_cast<uint8>(boundaryType));
}

void ATAPlayerController::onUnhoveredUi(UTAChunkUserWidget* input) noexcept
{
	const UiBoundaryType boundaryType = input->getBoundaryType();
	if (boundaryType <= UiBoundaryType::Hud)
	{
		return;
	}

	if (_hoveredUi == boundaryType)
	{
		_hoveredUi = UiBoundaryType::None;
		TA_LOG_DEV("unhovedUi = %d", static_cast<uint8>(boundaryType));
	}
}

// 각 메인위젯이 아닌 컨트롤러에서 수행하는 이유는 UI마다 다른 UI와 상호작용이 필요할 수 있기 때문에 모든 UI를 가지고 있는 PlayerController에서 수행한다.
void ATAPlayerController::onSlotReleased(UTAChunkUserWidget* slotParent, const ta::ItemSlotNo& slotNo) noexcept
{
	ATACharacter* targetCharacter = Cast<ATACharacter>(GetPawn());
	TA_ASSERT_DEV(nullptr != targetCharacter, "비정상");
	const ta::ActorKey targetActorKey = targetCharacter->getActorKey();

	// 검증
	ta::ClientActor* clientActor = targetCharacter->getActorFromActorManager();
	if (nullptr == clientActor)
	{
		TA_LOG_DEV("onSlotReleased => clientActor isn't exist");
		return;
	}

	const UiBoundaryType boundaryType = slotParent->getBoundaryType();

	switch (boundaryType)
	{
	case UiBoundaryType::Inventory:
		{
			if (_hoveredUi <= UiBoundaryType::Hud)
			{
				TA_LOG_DEV("onSlotReleased => Inventory to empty space // drop item");
				ta::ClientInventoryActorComponent* inventory = ta::GetActorComponent<ta::ClientInventoryActorComponent>(targetActorKey);
				if (nullptr == inventory)
				{
					TA_ASSERT_DEV(false, "Inventory가 없습니다. ActorKey : %d", targetActorKey.getKeyValue());
					return;
				}

				ta::ClientInventoryActorSystem* clientInventorySystem = ta::GetActorSystem<ta::ClientInventoryActorSystem>();
				if (false == clientInventorySystem->requestDropItem(inventory, slotNo, 1))
				{
					TA_ASSERT_DEV(false, "requestDropItem 실패");
				}
			}
		}
		break;
	default:
		break;
	}
}

void ATAPlayerController::BeginPlay()
{
	TA_LOG_DEV("player controller BeginPlay");
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	FVector2D viewportSize(0, 0);
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	TA_LOG_DEV("viewport 1 : %.1f, %.1f", viewportSize.X, viewportSize.Y);

	ATACharacter* targetCharacter = Cast<ATACharacter>(GetPawn());
	TA_ASSERT_DEV(nullptr != targetCharacter, "비정상");

	// 값똑같이 나온다.
	//FVector2D viewportSize2(GEngine->GameViewport->Viewport->GetSizeXY());
	//TA_LOG_DEV("viewport 2 : %.1f, %.1f", viewportSize2.X, viewportSize2.Y);

	{
		_hudUserWidget = CreateWidget<UTAHudUserWidget>(this, _hudUserWidgetClass);
		_hudUserWidget->initializeBase(targetCharacter, UiBoundaryType::None);
		_hudUserWidget->AddToViewport();
	}

	{
		_inventoryUserWidget = CreateWidget<UTAInventoryUserWidget>(this, _inventoryUserWidgetClass);
		_inventoryUserWidget->initializeBase(targetCharacter, UiBoundaryType::Inventory);
		UCanvasPanelSlot* inventoryCanvasPanelSlot = _hudUserWidget->addChildWidgetToPanel(_inventoryUserWidget);
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
	TA_LOG_DEV("player controller SetupInputComponent");
	Super::SetupInputComponent();
}