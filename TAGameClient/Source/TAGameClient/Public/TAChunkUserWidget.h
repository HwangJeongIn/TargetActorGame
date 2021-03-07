// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TACharacter.h"
#include "TACommonBase.h"
#include "TAChunkUserWidget.generated.h"


/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAChunkUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTAChunkUserWidget(const FObjectInitializer& ObjectInitializer);

	bool initializeBase(ATACharacter* ownerActor, const UiBoundaryType& boundaryType) noexcept;
	ATACharacter* getOwnerActor(void) noexcept;
	const UiBoundaryType getBoundaryType(void) noexcept;

protected:
	//virtual void NativeConstruct() override final;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override final;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override final;

private:
	UPROPERTY()
	UiBoundaryType _boundaryType;

	UPROPERTY()
	TWeakObjectPtr<ATACharacter> _ownerActor;

};
