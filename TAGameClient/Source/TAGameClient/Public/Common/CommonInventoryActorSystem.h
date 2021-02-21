#pragma once

#include "Common/ActorSystem.h"



namespace ta
{
	class CommonInventoryActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonInventoryActorSystem(void) noexcept;

		static const ActorSystemType getActorSystemType(void) noexcept;

	protected:
		CommonInventoryActorSystem(void) noexcept;

	};
}
