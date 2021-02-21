#pragma once

#include "Common/CommonInventoryActorComponent.h"


namespace ta
{
	class ClientInventoryActorComponent : public CommonInventoryActorComponent
	{
	public:
		ClientInventoryActorComponent(void) noexcept;
		virtual ~ClientInventoryActorComponent(void) noexcept;
	private:
	};
}
