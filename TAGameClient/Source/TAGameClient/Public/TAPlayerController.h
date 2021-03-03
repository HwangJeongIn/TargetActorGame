// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TAPlayerController.generated.h"


class UTAHUDUserWidget;
class UTAInventoryUserWidget;
class UTAInteractionMenuUserWidget;

/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATAPlayerController();

	virtual void PostInitializeComponents(void) override final;
	virtual void OnPossess(APawn* aPawn) override final;

	UTAHUDUserWidget* getHUDUserWidget(void) const;
	UTAInventoryUserWidget* getInventoryUserWidget(void) const;

	void openInventory(void) const noexcept;
	void closeInventory(void) const noexcept;
	
protected:
	virtual void BeginPlay() override final;
	virtual void SetupInputComponent() override final;

	// TSubclassOf 템플릿 클래스를 이용해 특정 멤버 변수나 변수를 특정 UObject/UClass의 하위 클래스로 한정지을 수 있다. 컴파일 타임
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAHUDUserWidget> _HUDUserWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInventoryUserWidget> _inventoryUserWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInteractionMenuUserWidget> _interactionMenuUserWidgetClass;

private:
	UPROPERTY()
	UTAHUDUserWidget* _HUDUserWidget;

	UPROPERTY()
	UTAInventoryUserWidget* _inventoryUserWidget;

	UPROPERTY()
	UTAInteractionMenuUserWidget* _interactionMenuUserWidget;
};	
