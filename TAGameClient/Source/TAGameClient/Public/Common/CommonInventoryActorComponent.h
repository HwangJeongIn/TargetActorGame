#pragma once

#include "Common/ActorComponent.h"
#include "Common/CommonBase.h"
#include <vector>

namespace ta
{
	class ItemSet;
	class Item;
}


namespace ta
{
	class CommonInventoryActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonInventoryActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;

		virtual ComponentData* packData_(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;

		ItemSet* getInventory_(void) noexcept;

		const Item* getItem_(const ItemSlotNo slotNo) const noexcept;

	protected:
		CommonInventoryActorComponent(void) noexcept;

	private:
		ItemSet* _inventory;

	};
}
