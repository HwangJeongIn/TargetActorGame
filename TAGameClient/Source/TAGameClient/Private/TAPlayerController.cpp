// Fill out your copyright notice in the Description page of Project Settings.


#include "TAPlayerController.h"
#include "TACharacter.h"
#include "TAHudUserWidget.h"
#include "TAInventoryUserWidget.h"
#include "TAInteractionMenuUserWidget.h"
#include "TAInteractionButtonUserWidget.h"
#include "TADialogUserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Common/CommonBase.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/KeyDefinition.h"
#include "Client/ClientActor.h"
#include "Client/ClientInventoryActorComponent.h"
#include "Client/ClientInventoryActorSystem.h"
#include "Engine.h"


ATAPlayerController::ATAPlayerController()
{
	{
		static ConstructorHelpers::FClassFinder<UTAHudUserWidget> GameHud_C(TEXT("/Game/_UI/GameHud.GameHud_C"));
		TA_ASSERT_DEV(true == GameHud_C.Succeeded(), "비정상");
		if (true == GameHud_C.Succeeded())
		{
			_hudUserWidgetClass = GameHud_C.Class;
		}
	}


	{
		static ConstructorHelpers::FClassFinder<UTAInventoryUserWidget> Inventory_C(TEXT("/Game/_UI/Inventory.Inventory_C"));
		TA_ASSERT_DEV(true == Inventory_C.Succeeded(), "비정상");
		if (true == Inventory_C.Succeeded())
		{
			_inventoryUserWidgetClass = Inventory_C.Class;
		}
	}


	{
		static ConstructorHelpers::FClassFinder<UTAInteractionMenuUserWidget> InteractionMenu_C(TEXT("/Game/_UI/InteractionMenu.InteractionMenu_C"));
		TA_ASSERT_DEV(true == InteractionMenu_C.Succeeded(), "비정상");
		if (true == InteractionMenu_C.Succeeded())
		{
			_interactionMenuUserWidgetClass = InteractionMenu_C.Class;
		}
	}

	{
		static ConstructorHelpers::FClassFinder<UTADialogUserWidget> Dialog_C(TEXT("/Game/_UI/Dialog.Dialog_C"));
		TA_ASSERT_DEV(true == Dialog_C.Succeeded(), "비정상");
		if (true == Dialog_C.Succeeded())
		{
			_dialogUserWidgetClass = Dialog_C.Class;
		}
	}

	_hoveredUi = TAUiBoundaryType::None;
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
	return _inventoryUserWidget->getVisibility();
}

void ATAPlayerController::setInventoryVisibility(const bool isVisible, const bool isForced /*= false*/) noexcept
{
	TA_LOG_DEV("setInventoryVisibility = %d", isVisible);
	_inventoryUserWidget->setVisibility(isVisible, isForced);
}

bool ATAPlayerController::getInteractionMenuVisibility(void) const noexcept
{
	return _interactionMenuUserWidget->getVisibility();
}

void ATAPlayerController::setInteractionMenuVisibility(const bool isVisible, const bool isForced /*= false*/) noexcept
{
	TA_LOG_DEV("setInteractionMenuVisibility = %d", isVisible);
	_interactionMenuUserWidget->setVisibility(isVisible, isForced);
}

bool ATAPlayerController::getDialogVisibility(void) const noexcept
{
	return _dialogUserWidget->getVisibility();
}

void ATAPlayerController::setDialogVisibility(const bool isVisible, const bool isForced /*= false*/) noexcept
{
	TA_LOG_DEV("setDialogVisibility = %d", isVisible);
	_dialogUserWidget->setVisibility(isVisible, isForced);
}

bool ATAPlayerController::openDialog(const ta::ActorKey& targetActorKey) noexcept
{
	TA_LOG_DEV("openDialog = %d", targetActorKey.getKeyValue());
	setDialogVisibility(true);

	if (false == _dialogUserWidget->openDialog(targetActorKey))
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	return true;
}

bool ATAPlayerController::closeDialog(void) noexcept
{
	TA_LOG_DEV("closeDialog");
	setDialogVisibility(false);

	_dialogUserWidget->closeDialog();
	return true;
}

void ATAPlayerController::onHoveredUi(UTAChunkUserWidget* input) noexcept
{
	const TAUiBoundaryType boundaryType = input->getBoundaryType();
	if (boundaryType <= TAUiBoundaryType::Hud)
	{
		return;
	}

	_hoveredUi = boundaryType;
	TA_LOG_DEV("hovedUi = %d", static_cast<uint8>(boundaryType));
}

void ATAPlayerController::onUnhoveredUi(UTAChunkUserWidget* input) noexcept
{
	const TAUiBoundaryType boundaryType = input->getBoundaryType();
	if (boundaryType <= TAUiBoundaryType::Hud)
	{
		return;
	}

	if (_hoveredUi == boundaryType)
	{
		_hoveredUi = TAUiBoundaryType::None;
		TA_LOG_DEV("unhovedUi = %d", static_cast<uint8>(boundaryType));
	}
}

// 각 메인위젯이 아닌 컨트롤러에서 수행하는 이유는 UI마다 다른 UI와 상호작용이 필요할 수 있기 때문에 모든 UI를 가지고 있는 PlayerController에서 수행한다.
void ATAPlayerController::onSlotReleased(UTAChunkUserWidget* slotParent, const ta::ItemSlotNo& slotNo) noexcept
{
	ATACharacter* targetCharacter = Cast<ATACharacter>(GetPawn());
	TA_ASSERT_DEV(nullptr != targetCharacter, "비정상");
	const ta::ActorKey targetActorKey = targetCharacter->getActorKey();

	// 검증 굳이 필요없을듯
	//ta::ClientActor* clientActor = targetCharacter->getActorFromActorManager();
	//if (nullptr == clientActor)
	//{
	//	TA_LOG_DEV("onSlotReleased => clientActor isn't exist");
	//	return;
	//}

	const TAUiBoundaryType boundaryType = slotParent->getBoundaryType();

	switch (boundaryType)
	{
	case TAUiBoundaryType::Inventory:
		{
			if (_hoveredUi <= TAUiBoundaryType::Hud)
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


bool ATAPlayerController::setInteractionObjectsByInteractionType(const ta::ActorKey& targetActor, const uint8& buttonIndex, ta::InteractionType interactionType) noexcept
{
	TWeakObjectPtr<UTAInteractionButtonUserWidget> interactionButton = getInteractionButton(buttonIndex);
	if (false == interactionButton.IsValid())
	{
		TA_ASSERT_DEV(false, "비정상 %d", buttonIndex);
		return false;
	}

	ATACharacter* myCharacter = Cast<ATACharacter>(GetPawn());
	if (nullptr == myCharacter)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (false == interactionButton->setInteractionObjectsByInteractionType(myCharacter->getActorKey(), targetActor, interactionType))
	{
		TA_ASSERT_DEV(false, "비정상 %d", buttonIndex);
		return false;
	}

	return true;
}

bool ATAPlayerController::executeInteractionButton(const uint8& buttonIndex) noexcept
{
	TWeakObjectPtr<UTAInteractionButtonUserWidget> interactionButton = getInteractionButton(buttonIndex);
	if (false == interactionButton.IsValid())
	{
		TA_ASSERT_DEV(false, "비정상 %d", buttonIndex);
		return false;
	}

	if (false == interactionButton->doInteraction())
	{
		TA_ASSERT_DEV(false, "비정상 %d", buttonIndex);
		return false;
	}

	return true;
}

TWeakObjectPtr<UTAInteractionButtonUserWidget> ATAPlayerController::getInteractionButton(const uint8& buttonIndex) noexcept
{
	return _interactionMenuUserWidget->getInteractionButton(buttonIndex);
}

void ATAPlayerController::setVisibleButtons(const int32& count) noexcept
{
	_interactionMenuUserWidget->setVisibleButtons(count);
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
		_hudUserWidget->initializeBase(targetCharacter, TAUiBoundaryType::None);
		_hudUserWidget->AddToViewport();
	}

	{
		_inventoryUserWidget = CreateWidget<UTAInventoryUserWidget>(this, _inventoryUserWidgetClass);
		_inventoryUserWidget->initializeBase(targetCharacter, TAUiBoundaryType::Inventory);
		UCanvasPanelSlot* inventoryCanvasPanelSlot = _hudUserWidget->addChildWidgetToPanel(_inventoryUserWidget);
		if (nullptr == inventoryCanvasPanelSlot)
		{
			TA_ASSERT_DEV(false, "비정상");
		}
		else // 부모인 Hud기준 상대좌표를 지정한다.
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
			setInventoryVisibility(false, true);
			//_inventoryUserWidget->SetVisibility(ESlateVisibility::Hidden);
		}

	}

	{
		_interactionMenuUserWidget = CreateWidget<UTAInteractionMenuUserWidget>(this, _interactionMenuUserWidgetClass);
		_interactionMenuUserWidget->initializeBase(targetCharacter, TAUiBoundaryType::None);
		UCanvasPanelSlot* interactionMenuCanvasPanelSlot = _hudUserWidget->addChildWidgetToPanel(_interactionMenuUserWidget);
		if (nullptr == interactionMenuCanvasPanelSlot)
		{
			TA_ASSERT_DEV(false, "비정상");
		}
		else // 부모인 Hud기준 상대좌표를 지정한다.
		{
			interactionMenuCanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f)); // 중앙위치로
			interactionMenuCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f)); 
			interactionMenuCanvasPanelSlot->SetPosition(FVector2D(0.0f, (viewportSize.X * 0.05f) )); // 화면의 1/4사이즈로 인벤토리 할 예정 피벗이 중앙이므로 2로 나눈값으로 위치를 잡는다.
			//interactionMenuCanvasPanelSlot->SetSize(FVector2D(viewportSize.X / 2.0f, viewportSize.Y / 2.0f));
			interactionMenuCanvasPanelSlot->SetSize(FVector2D(viewportSize.X * 0.3f, viewportSize.Y * 0.225f));

			setInteractionMenuVisibility(false, true);
		}
	}

	{
		_dialogUserWidget = CreateWidget<UTADialogUserWidget>(this, _dialogUserWidgetClass);
		_dialogUserWidget->initializeBase(targetCharacter, TAUiBoundaryType::None);
		UCanvasPanelSlot* dialogCanvasPanelSlot = _hudUserWidget->addChildWidgetToPanel(_dialogUserWidget);
		if (nullptr == dialogCanvasPanelSlot)
		{
			TA_ASSERT_DEV(false, "비정상");
		}
		else // 부모인 Hud기준 상대좌표를 지정한다.
		{
			dialogCanvasPanelSlot->SetAnchors(FAnchors(0.5f, 1.0f));
			dialogCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			dialogCanvasPanelSlot->SetPosition(FVector2D(0, -(viewportSize.Y * 0.225f * 0.5f)));
			dialogCanvasPanelSlot->SetSize(FVector2D(viewportSize.X, viewportSize.Y * 0.225f));

			setDialogVisibility(true, true);
		}
	}


}

void ATAPlayerController::SetupInputComponent()
{
	TA_LOG_DEV("player controller SetupInputComponent");
	Super::SetupInputComponent();
}