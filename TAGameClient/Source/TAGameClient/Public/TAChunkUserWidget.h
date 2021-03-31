// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TACharacter.h"
#include "TACommonBase.h"
#include "TAChunkUserWidget.generated.h"


class AController;
/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTAChunkUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTAChunkUserWidget(const FObjectInitializer& ObjectInitializer);

	bool initializeBase(ATACharacter* ownerActor, const TAUiBoundaryType& boundaryType) noexcept;
	ATACharacter* getOwnerActor(void) noexcept;
	AController* getOwnerController(void) noexcept;
	const TAUiBoundaryType getBoundaryType(void) noexcept;

	// UE4 자체적으로 버그있는것같다. 상속받은 객체를 여기서 Visibility 조절하면 비정상적인 상황이있다.(처음시작할때 Hidden으로 해놔도 가려지지않는다.)
	//const bool getVisibility(void) const noexcept;
	//void setVisibility(const bool isVisible) noexcept;

protected:
	//virtual void NativeConstruct() override final;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override final;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override final;

private:
	UPROPERTY()
	TAUiBoundaryType _boundaryType;

	UPROPERTY()
	TWeakObjectPtr<ATACharacter> _ownerActor;

};
