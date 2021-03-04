#pragma once

#include "Common/CommonInventoryActorComponent.h"


namespace ta
{
	class ClientInventoryActorComponent : public CommonInventoryActorComponent
	{
	public:
		ClientInventoryActorComponent(void) noexcept;
		virtual ~ClientInventoryActorComponent(void) noexcept;

		virtual bool initializeData(const ComponentData* componentData) noexcept override final;
	private:
	};
}
