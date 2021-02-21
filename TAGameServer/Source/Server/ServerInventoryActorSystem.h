#pragma once

#include "Common/CommonInventoryActorSystem.h"


namespace ta
{
	class ActorKey;
}


namespace ta
{
	class ServerInventoryActorSystem : public CommonInventoryActorSystem
	{
	public:
		ServerInventoryActorSystem(void) noexcept;
		virtual ~ServerInventoryActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;

	};
}
