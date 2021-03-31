// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "TAInteractionObject.h"
#include "Common/CommonDataTypeDefinition.h"
#include "TAInteractionButtonUserWidget.generated.h"


class UTextBlock;
class UButton;
class UImage;
class UTAInteractionObject;
class TAInteractionObjectParameter;
class UTAInteractionMenuUserWidget;

namespace ta
{
	class ActorKey;
}


/**
 *
 */
UCLASS()
class TAGAMECLIENT_API UTAInteractionButtonUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTAInteractionButtonUserWidget(const FObjectInitializer& ObjectInitializer);

	bool doInteraction(void) const noexcept;
	bool setInteractionObjectsByInteractionType(const ta::ActorKey& myActorKey, const ta::ActorKey& targetActorKey, const ta::InteractionType& interactionType) noexcept;

	void setInfo(UTAInteractionMenuUserWidget* ownerInteractionMenu) noexcept;
	
protected:

	virtual void NativeConstruct() override final;

private:

	void clearInteractionObjects(void) noexcept;

	UFUNCTION()
	void onInteractionButtonPressed() noexcept;

private:

	UPROPERTY()
	TWeakObjectPtr<UTAInteractionMenuUserWidget> _ownerInteractionMenu;

	UPROPERTY()
	UButton* _interactionButton;

	UPROPERTY()
	UImage* _interactionImage;

	UPROPERTY()
	UTextBlock* _interactionName;

	UPROPERTY()
	TArray<UTAInteractionObject*> _interactionObjects;
};
