// Fill out your copyright notice in the Description page of Project Settings.


#include "TAInventorySlotUserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Common/CommonBase.h"
#include "Client/ClientInventoryActorComponent.h"



void UTAInventorySlotUserWidget::setSlotNo(const ta::ItemSlotNo slotNo) noexcept
{
	_slotNo = slotNo;
}

//void refresh(ta::ClientInventoryActorComponent* inventoryComponent) noexcept
//{
//	ta::ScopedLock inventoryLock(inventoryComponent);
//
//	//Item* a = InventoryComponent->getItem(slotNo);
//	//a->getImage();
//	//a->getStackCount();
//}


void UTAInventorySlotUserWidget::refresh_(ta::ClientInventoryActorComponent* inventoryComponent) noexcept
{
	//Item* a = InventoryComponent->getItem(slotNo);
	//a->getImage();
	//a->getStackCount();
}

void UTAInventorySlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();


	_slotButton = Cast<UButton>(GetWidgetFromName(TEXT("inventorySlotButton")));
	TA_ASSERT_DEV(nullptr != _slotButton, "비정상");
	_slotButton->OnClicked.AddDynamic(this, &UTAInventorySlotUserWidget::onSlotClicked);

	_slotImage = Cast<UImage>(GetWidgetFromName(TEXT("inventorySlotImage")));
	TA_ASSERT_DEV(nullptr != _slotImage, "비정상")
	
	_slotStackCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("inventorySlotStackCount")));
	TA_ASSERT_DEV(nullptr != _slotStackCount, "비정상")
}

void UTAInventorySlotUserWidget::onSlotClicked() noexcept
{
	// 인벤토리 슬롯하나 클릭했을때
}