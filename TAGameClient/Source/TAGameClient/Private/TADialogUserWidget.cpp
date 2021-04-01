// Fill out your copyright notice in the Description page of Project Settings.


#include "TADialogUserWidget.h"
#include "Components/MultiLineEditableText.h"
#include "Components/EditableText.h"
#include "Common/KeyDefinition.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/GameData.h"
#include "Client/ClientActorManager.h"


UTADialogUserWidget::UTADialogUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTADialogUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	_dialogActorName = Cast<UEditableText>(GetWidgetFromName(TEXT("dialogActorName")));
	TA_ASSERT_DEV(nullptr != _dialogActorName, "비정상");

	_dialogTextBody = Cast<UMultiLineEditableText>(GetWidgetFromName(TEXT("dialogTextBody")));
	TA_ASSERT_DEV(nullptr != _dialogTextBody, "비정상");
}

bool UTADialogUserWidget::openDialog(const ta::ActorKey& targetActorKey) noexcept
{
	if (false == targetActorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}
	
	// 컴포넌트 받았는데 유효하지 않으면 nullptr 리턴한다. // 현재 액터가 존재할때만 유효하다. => 검증
	ta::CommonCharacterActorComponent* characterCom = ta::GetActorComponent<ta::CommonCharacterActorComponent>(targetActorKey);
	if (nullptr == characterCom)
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	const ta::CharacterGameData* characterGameData = nullptr;
	{
		ta::ScopedLock characterComLock(characterCom, true);
		characterGameData = characterCom->getCharacterGameData_();
	}

	_targetActorKey = targetActorKey;

	_dialogActorName->SetText(FText::FromString(UTF8_TO_TCHAR(characterGameData->_name.c_str())));
	_dialogTextBody->SetText(FText::FromString(UTF8_TO_TCHAR(characterGameData->_openDialog.c_str())));

	return true;
}

void UTADialogUserWidget::closeDialog(void) noexcept
{
	_targetActorKey.clear();
}
