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
	// 처음에(playercontroller(begine play) 에서 생성시) hidden으로 플래그가 되어있어서 hide를 안시켰는데, 나중에 보니까 visible로 되어있다. => 현재상황에 상관없이 켜고 끄는걸 만든다.
	const bool getVisibility(void) const noexcept;
	void setVisibility(const bool isVisible, const bool isForced = false) noexcept;

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
