// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAChunkUserWidget.h"
#include "TAInteractionMenuUserWidget.generated.h"


class UTAInteractionButtonUserWidget;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAInteractionMenuUserWidget : public UTAChunkUserWidget
{
	GENERATED_BODY()
public:
	UTAInteractionMenuUserWidget(const FObjectInitializer& ObjectInitializer);

	bool initializeInteractionButtons(void) noexcept;
	TWeakObjectPtr<UTAInteractionButtonUserWidget> getInteractionButton(const uint8& index) noexcept;
	
	void setVisibleButtons(const int32& count) noexcept;

protected:
	virtual void NativeConstruct() override final;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInteractionButtonUserWidget> _interactionButtonClass;

private:
	UPROPERTY()
	UHorizontalBox* _interactionButtonWrap;
};
