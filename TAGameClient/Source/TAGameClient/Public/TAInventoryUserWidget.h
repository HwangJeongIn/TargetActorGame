// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAChunkUserWidget.h"
#include "Common/CommonBase.h"
#include "TAInventoryUserWidget.generated.h"



class UUniformGridPanel;
class UTAInventorySlotUserWidget;

namespace ta
{
	class ActorKey;
}

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAInventoryUserWidget : public UTAChunkUserWidget
{
	GENERATED_BODY()
	
public:
	UTAInventoryUserWidget(const FObjectInitializer& ObjectInitializer);

	bool checkValid(void) const noexcept;	
	bool refreshSlot(const ta::ActorKey& target, const ta::ItemSlotNo& slotNo) noexcept;
	bool refreshSlots(const ta::ActorKey& target) noexcept;

	bool setInventorySlotCount(const int32 capacity) noexcept;
	bool setPressedSlot(const ta::ItemSlotNo& slotNo) noexcept;
	void releaseSlot(void) noexcept;

protected:
	virtual void NativeConstruct() override final;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInventorySlotUserWidget> _inventorySlotClass;

private:

	UPROPERTY()
	int32 _pressedSlot;

	UPROPERTY()
	bool _isValid;

	UPROPERTY()
	UUniformGridPanel* _inventoryGrid;

	UPROPERTY()
	TArray<UTAInventorySlotUserWidget*> _inventorySlots;
};



