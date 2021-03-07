#pragma once

#include "Common/CommonInventoryActorSystem.h"


namespace ta
{
	class ActorKey;
	class CommonInventoryActorComponent;
}


namespace ta
{
	class ClientInventoryActorSystem : public CommonInventoryActorSystem
	{
	public:
		ClientInventoryActorSystem(void) noexcept;
		virtual ~ClientInventoryActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;


		bool requestDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo slotNo, const int32 stackCount = 1) const noexcept;
		bool requestUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo slotNo, const int32 stackCount = 1) const noexcept;

		bool respondDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept;
		bool respondUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept;

	};
}
