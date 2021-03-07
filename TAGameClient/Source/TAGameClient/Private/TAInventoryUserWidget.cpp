// Fill out your copyright notice in the Description page of Project Settings.


#include "TAInventoryUserWidget.h"
#include "TAInventorySlotUserWidget.h"
#include "TAPlayerController.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Common/CommonBase.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Client/ClientInventoryActorComponent.h"



UTAInventoryUserWidget::UTAInventoryUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTAInventorySlotUserWidget> InventorySlot_C(TEXT("/Game/UI/InventorySlot.InventorySlot_C"));
	TA_ASSERT_DEV(true == InventorySlot_C.Succeeded(), "비정상");
	if (true == InventorySlot_C.Succeeded())
	{
		_inventorySlotClass = InventorySlot_C.Class;
	}

	_isValid = false;
	_pressedSlot = -1;
}

void UTAInventoryUserWidget::NativeConstruct()
{
	_inventoryGrid = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("inventoryGrid")));
	TA_ASSERT_DEV(nullptr != _inventoryGrid, "비정상")
}

bool UTAInventoryUserWidget::checkValid(void) const noexcept
{
	return  _isValid;
}

bool UTAInventoryUserWidget::refreshSlot(const ta::ActorKey& target, const ta::ItemSlotNo& slotNo) noexcept
{
	if (false == checkValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (_inventorySlots.Num() <= slotNo
		|| slotNo < 0)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
	
	ta::ClientInventoryActorComponent* inventory = ta::GetActorComponent<ta::ClientInventoryActorComponent>(target);
	if (nullptr == inventory)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
	ta::ScopedLock inventoryLock(inventory);
	_inventorySlots[slotNo]->refresh_(inventory);

	return true;
}

bool UTAInventoryUserWidget::refreshSlots(const ta::ActorKey& target) noexcept
{
	if (false == checkValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	ta::ClientInventoryActorComponent* inventory = ta::GetActorComponent<ta::ClientInventoryActorComponent>(target);
	if (nullptr == inventory)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	ta::ScopedLock inventoryLock(inventory);

	const uint32 count = _inventorySlots.Num();
	for (uint32 index = 0; index < count; ++index)
	{
		_inventorySlots[index]->refresh_(inventory);
	}

	//for (UTAInventorySlotUserWidget*& inventorySlot : _inventorySlots)
	//{
	//	inventorySlot;
	//}

	return true;
}


bool UTAInventoryUserWidget::setInventorySlotCount(const int32 count) noexcept
{
	if (true == checkValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (nullptr == WidgetTree)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	_isValid = true;

	const uint32 maxColumnSize = 5;
	uint32 currentColumn = 0;
	uint32 currentRow = 0;
	UTAInventorySlotUserWidget* currentSlot = nullptr;
	FString slotString;
	//UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	for (int32 index = 0; index < count; ++index)
	{
		currentColumn = index % maxColumnSize;
		currentRow = index / maxColumnSize;

		slotString = FString::Printf(TEXT("inventorySlot_%d"), index);
		//slotString = "inventorySlot_";
		//slotString  += FString::FromInt(index);

		currentSlot = WidgetTree->ConstructWidget<UTAInventorySlotUserWidget>(_inventorySlotClass, *slotString);
		currentSlot->Padding = FMargin(3.0f);
		currentSlot->setInfo(this, index);
		_inventoryGrid->AddChildToUniformGrid(currentSlot, currentRow, currentColumn);
		_inventorySlots.Add(currentSlot);

		//UTAInventorySlotUserWidget* testSlot = WidgetTree->ConstructWidget<UTAInventorySlotUserWidget>(_inventorySlotClass, TEXT("test231")); 
		////ExampleButton->OnClicked.AddDynamic(this, &MyUserWidget::OnButtonClicked); // Example click binding.
		//_inventory->AddChild(testSlot);

	}

	return true;
	

	//UTAInventorySlotUserWidget* testSlot = CreateWidget<UTAInventorySlotUserWidget>(GetWorld(), _inventorySlotClass, TEXT("test231"));
	//testSlot->AddToViewport();
	//_inventory->AddChild(testSlot);

}

bool UTAInventoryUserWidget::setPressedSlot(const ta::ItemSlotNo& slotNo) noexcept
{
	if (_inventorySlots.Num() <= slotNo
		|| slotNo < 0)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	_pressedSlot = slotNo;
	return true;
}

void UTAInventoryUserWidget::releaseSlot(void) noexcept
{
	ATAPlayerController* playerController = Cast<ATAPlayerController>(getOwnerActor()->GetController());
	if (nullptr == playerController)
	{
		TA_ASSERT_DEV(false, "비정상");
		return;
	}

	TA_ASSERT_DEV(-1 != _pressedSlot, "비정상");
	playerController->onSlotReleased(this, _pressedSlot);
	_pressedSlot = -1;
}