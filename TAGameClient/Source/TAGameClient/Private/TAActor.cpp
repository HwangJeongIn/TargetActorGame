// Fill out your copyright notice in the Description page of Project Settings.


#include "TAActor.h"
#include "Client/ClientApp.h"
#include "Client/ClientActor.h"
#include "Client/ClientActorManager.h"




TAActor::TAActor()
{
	_actorKey.clear();
}

TAActor::~TAActor()
{
}

void TAActor::resetActorKey(void) noexcept
{
	_actorKey.clear();
}

bool TAActor::setActorKey(const ta::ActorKey& actorKey) noexcept
{
	if (false == actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	if (true == _actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return false;
	}

	_actorKey = actorKey;
	return true;
}

const ta::ActorKey& TAActor::getActorKey(void) const noexcept
{
	return _actorKey;
}

ta::ClientActor* TAActor::getActorFromActorManager(void) const noexcept
{
	if (false == _actorKey.isValid())
	{
		TA_ASSERT_DEV(false, "비정상");
		return nullptr;
	}

	return static_cast<ta::ClientActor*>(ta::GetActorManager()->getActor(_actorKey));
}
