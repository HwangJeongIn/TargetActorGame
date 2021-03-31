// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TACommonBase.h"
#include "Common/KeyDefinition.h"
#include "TAInteractionObject.generated.h"

/**
 * 
 */


UCLASS()
class TAGAMECLIENT_API UTAInteractionObject : public UObject
{
	GENERATED_BODY()
	
public:
	UTAInteractionObject();

	virtual bool execute(void) const noexcept;

	void setInfo(ta::ActorKey myActorKey, ta::ActorKey targetActorKey) noexcept;

protected:
	UPROPERTY()
	TAInteractionObjectType _interactionObjectType;

	ta::ActorKey _myActorKey;
	ta::ActorKey _targetActorKey;
};


UCLASS()
class TAGAMECLIENT_API UTAInteractionObjectLookAt : public UTAInteractionObject
{
	GENERATED_BODY()
	
public:
	UTAInteractionObjectLookAt();

	virtual bool execute(void) const noexcept override final;
};

UCLASS()
class TAGAMECLIENT_API UTAInteractionObjectStopAndLookAt : public UTAInteractionObject
{
	GENERATED_BODY()
	
public:
	UTAInteractionObjectStopAndLookAt();

	virtual bool execute(void) const noexcept override final;
};


UCLASS()
class TAGAMECLIENT_API UTAInteractionObjectOpenDialog : public UTAInteractionObject
{
	GENERATED_BODY()
	
public:
	UTAInteractionObjectOpenDialog();

	virtual bool execute(void) const noexcept override final;
};
