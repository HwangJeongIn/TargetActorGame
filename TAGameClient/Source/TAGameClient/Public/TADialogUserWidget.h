// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAChunkUserWidget.h"
#include "Common/KeyDefinition.h"
#include "TADialogUserWidget.generated.h"


class UEditableText;
class UMultiLineEditableText;

namespace ta
{
	class ActorKey;
}
/**
 * 
 */
UCLASS()
class TAGAMECLIENT_API UTADialogUserWidget : public UTAChunkUserWidget
{
	GENERATED_BODY()
public:
	UTADialogUserWidget(const FObjectInitializer& ObjectInitializer);

	bool openDialog(const ta::ActorKey& targetActorKey) noexcept;
	void closeDialog(void) noexcept;

protected:
	virtual void NativeConstruct() override final;


private:
	UPROPERTY()
	UEditableText* _dialogActorName;

	UPROPERTY()
	UMultiLineEditableText* _dialogTextBody;

	ta::ActorKey _targetActorKey;
};
