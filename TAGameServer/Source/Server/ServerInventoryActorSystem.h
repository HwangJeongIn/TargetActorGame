#pragma once

#include "Common/CommonInventoryActorSystem.h"


namespace ta
{
	class ActorKey;
	class CommonInventoryActorComponent;
}


namespace ta
{
	class ServerInventoryActorSystem : public CommonInventoryActorSystem
	{
	public:
		ServerInventoryActorSystem(void) noexcept;
		virtual ~ServerInventoryActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;

		bool respondDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept;
		bool respondUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept;
	};
}
