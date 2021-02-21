#pragma once

#include "Common/CommonInventoryActorSystem.h"


namespace ta
{
	class ActorKey;
}


namespace ta
{
	class ClientInventoryActorSystem : public CommonInventoryActorSystem
	{
	public:
		ClientInventoryActorSystem(void) noexcept;
		virtual ~ClientInventoryActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;

	};
}
