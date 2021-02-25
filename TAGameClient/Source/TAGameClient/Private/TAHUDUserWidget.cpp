// Fill out your copyright notice in the Description page of Project Settings.


#include "TAHUDUserWidget.h"
#include "TAInventorySlotUserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Common/CommonBase.h"


UTAHUDUserWidget::UTAHUDUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTAInventorySlotUserWidget> InventorySlot_C(TEXT("/Game/UI/InventorySlot.InventorySlot_C"));
	TA_ASSERT_DEV(true == InventorySlot_C.Succeeded(), "비정상");
	if (true == InventorySlot_C.Succeeded())
	{
		_inventorySlotClass = InventorySlot_C.Class;
	}
}

void UTAHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	_HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	TA_ASSERT_DEV(nullptr != _HPBar, "비정상")

	_EPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("EPBar")));
	TA_ASSERT_DEV(nullptr != _EPBar, "비정상")

	_MPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("MPBar")));
	TA_ASSERT_DEV(nullptr != _MPBar, "비정상")

	_inventory = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("inventory")));
	TA_ASSERT_DEV(nullptr != _inventory, "비정상")


}


bool UTAHUDUserWidget::setInventorySlotCount(const uint32 count) noexcept
{
	//UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	const uint32 maxColumnSize = 5;
	uint32 currentColumn = 0;
	uint32 currentRow = 0;
	UTAInventorySlotUserWidget* currentSlot = nullptr;
	FString slotString;

	if (WidgetTree)
	{
		for (uint32 index = 0; index < count; ++index)
		{
			currentColumn = index % maxColumnSize;
			currentRow = index / maxColumnSize;

			slotString = FString::Printf(TEXT("inventorySlot_%d"), index);
			//slotString = "inventorySlot_";
			//slotString  += FString::FromInt(index);

			currentSlot = WidgetTree->ConstructWidget<UTAInventorySlotUserWidget>(_inventorySlotClass, *slotString);
			currentSlot->Padding = FMargin(3.0f);
			_inventory->AddChildToUniformGrid(currentSlot, currentRow, currentColumn);


			//UTAInventorySlotUserWidget* testSlot = WidgetTree->ConstructWidget<UTAInventorySlotUserWidget>(_inventorySlotClass, TEXT("test231")); 
			////ExampleButton->OnClicked.AddDynamic(this, &MyUserWidget::OnButtonClicked); // Example click binding.
			//_inventory->AddChild(testSlot);
		
		}

		return true;
	}

	//UTAInventorySlotUserWidget* testSlot = CreateWidget<UTAInventorySlotUserWidget>(GetWorld(), _inventorySlotClass, TEXT("test231"));
	//testSlot->AddToViewport();
	//_inventory->AddChild(testSlot);


	return false;
}