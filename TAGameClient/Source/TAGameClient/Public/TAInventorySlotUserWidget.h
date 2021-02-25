// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TAInventorySlotUserWidget.generated.h"



class UProgressBar;
class UTextBlock;
class UButton;

/**
 *
 */
UCLASS()
class TAGAMECLIENT_API UTAInventorySlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override final;

private:
	//UPROPERTY()
//UProgressBar* _HPBar;

	//UPROPERTY()
//UProgressBar* _HPBar;

	//UPROPERTY()
//UProgressBar* _EPBar;

	//UPROPERTY()
//UProgressBar* _MPBar;

};
