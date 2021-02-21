#pragma once

#include "Common/ActorComponent.h"
#include <vector>

namespace ta
{
	class ItemSet;
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
	protected:
		CommonInventoryActorComponent(void) noexcept;

	private:
		ItemSet* _inventory;

	};
}
