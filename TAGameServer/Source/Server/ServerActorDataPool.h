#pragma once

#include "Common/CommonBase.h"
#include "Common/ActorDataPool.h"


namespace ta
{
	class ServerActorDataPool : public ActorDataPool
	{
	public:
		ServerActorDataPool(void) noexcept;
		virtual ~ServerActorDataPool(void) noexcept;

		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		virtual CommonActor* getActor(const ActorKey& actorKey) noexcept override final;
		virtual ActorComponent* getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept override final;
	};
}
