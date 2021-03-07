// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/CommonBase.h"
#include "TAInventorySlotUserWidget.generated.h"


class UTextBlock;
class UButton;
class UImage;
class UTAInventoryUserWidget;

namespace ta
{
	class ClientInventoryActorComponent;
	typedef ItemSlotNo;
}
/**
 *
 */
UCLASS()
class TAGAMECLIENT_API UTAInventorySlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTAInventorySlotUserWidget(const FObjectInitializer& ObjectInitializer);

	void setInfo(UTAInventoryUserWidget* ownerInventory, const ta::ItemSlotNo slotNo) noexcept;
	//void setSlotNo(const ta::ItemSlotNo slotNo) noexcept;

	//void refresh(ta::ClientInventoryActorComponent* inventoryComponent) noexcept;
	void refresh_(ta::ClientInventoryActorComponent* inventoryComponent) noexcept;

	
protected:

	virtual void NativeConstruct() override final;

private:

	UFUNCTION()
	void onSlotClicked() noexcept;

	UFUNCTION()
	void onSlotPressed() noexcept;

	UFUNCTION()
	void onSlotReleased() noexcept;

private:
	UPROPERTY()
	UTAInventoryUserWidget* _ownerInventory;
 
	UPROPERTY()
	int32 _slotNo; // ta::ItemSlotNo _slotNo로 해야하는데 UPROPERTY()를 붙이면 지원되지 않는 것 같다

	UPROPERTY()
	UButton* _slotButton;

	UPROPERTY()
	UImage* _slotImage;

	UPROPERTY()
	UTextBlock* _slotStackCount;
};
