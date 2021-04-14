// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Common/KeyDefinition.h"
#include "CoreMinimal.h"

namespace ta
{
	class ClientActor;
}

/**
 * 
 */
class TAGAMECLIENT_API TAActor
{
public:

	TAActor();
	~TAActor();

	void resetActorKey(void) noexcept;
	bool setActorKey(const ta::ActorKey& actorKey) noexcept;
	const ta::ActorKey& getActorKey(void) const noexcept;
	ta::ClientActor* getActorFromActorManager(void) const noexcept;

private:

	ta::ActorKey _actorKey;

};
