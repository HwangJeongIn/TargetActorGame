// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TAHUDUserWidget.generated.h"


class UProgressBar;
class UTextBlock;
class UUniformGridPanel;
class UTAInventorySlotUserWidget;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTAHUDUserWidget(const FObjectInitializer& ObjectInitializer);
	bool setInventorySlotCount(const uint32 count) noexcept;

protected:

	virtual void NativeConstruct() override final;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInventorySlotUserWidget> _inventorySlotClass;

private:

	UPROPERTY()
	UProgressBar* _HPBar;
	
	UPROPERTY()
	UProgressBar* _EPBar;
	
	UPROPERTY()
	UProgressBar* _MPBar;
	
/////////////////////////////////////////////

	UPROPERTY()
	UUniformGridPanel* _inventory;

	//UPROPERTY()
	//UTAInventorySlotUserWidget* _inventorySlot;

};
