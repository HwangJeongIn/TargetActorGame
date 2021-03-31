// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TACommonBase.h"
#include "Common/CommonDataTypeDefinition.h"
#include "TAPlayerController.generated.h"


class UTAHudUserWidget;
class UTAInventoryUserWidget;
class UTAInteractionMenuUserWidget;
class UTAChunkUserWidget;
class UTAInteractionButtonUserWidget;


namespace ta
{
	typedef int32 ItemSlotNo;
	class ActorKey;
}
/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API ATAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// 안에 정의하지 못한다. ue4자체적으로 오버해드 때문에 막아둠
	//UENUM(/*BlueprintType*/)
	//enum class TAUiBoundaryType: uint8
	//{
	//	None = 0
	//	, Inventory
	//};


	ATAPlayerController();

	virtual void PostInitializeComponents(void) override final;
	virtual void OnPossess(APawn* aPawn) override final;

	UTAHudUserWidget* getHudUserWidget(void) const;
	UTAInventoryUserWidget* getInventoryUserWidget(void) const;

	bool getInventoryVisibility(void) const noexcept;
	void setInventoryVisibility(bool isVisible) const noexcept;

	bool getInteractionMenuVisibility(void) const noexcept;
	void setInteractionMenuVisibility(bool isVisible) const noexcept;

	void onHoveredUi(UTAChunkUserWidget* input) noexcept;
	void onUnhoveredUi(UTAChunkUserWidget* input) noexcept;
	
	void onSlotReleased(UTAChunkUserWidget* slotParent, const ta::ItemSlotNo& slotNo) noexcept;

	bool setInteractionObjectsByInteractionType(const ta::ActorKey& targetActor, const uint8& buttonIndex, ta::InteractionType interactionType) noexcept;
	bool executeInteractionButton(const uint8& buttonIndex) noexcept;
	TWeakObjectPtr<UTAInteractionButtonUserWidget> getInteractionButton(const uint8& buttonIndex) noexcept;

	void setVisibleButtons(const int32& count) noexcept;

protected:
	virtual void BeginPlay() override final;
	virtual void SetupInputComponent() override final;


protected:

	// TSubclassOf 템플릿 클래스를 이용해 특정 멤버 변수나 변수를 특정 UObject/UClass의 하위 클래스로 한정지을 수 있다. 컴파일 타임
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAHudUserWidget> _hudUserWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInventoryUserWidget> _inventoryUserWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UTAInteractionMenuUserWidget> _interactionMenuUserWidgetClass;

private:

	// ui
	UPROPERTY()
	TAUiBoundaryType _hoveredUi;

	UPROPERTY()
	UTAHudUserWidget* _hudUserWidget;

	UPROPERTY()
	UTAInventoryUserWidget* _inventoryUserWidget;

	UPROPERTY()
	UTAInteractionMenuUserWidget* _interactionMenuUserWidget;
};	
