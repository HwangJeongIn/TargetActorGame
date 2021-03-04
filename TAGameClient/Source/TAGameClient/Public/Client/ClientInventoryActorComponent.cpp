#ifndef TA_SERVER
#include "TAGameEvent.h"
#endif

#include "Client/ClientInventoryActorComponent.h"
#include "Common/ComponentData.h"

namespace ta
{
	ClientInventoryActorComponent::ClientInventoryActorComponent(void) noexcept
	{
	}

	ClientInventoryActorComponent::~ClientInventoryActorComponent(void) noexcept
	{
	}

	bool ClientInventoryActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		if (false == CommonInventoryActorComponent::initializeData(componentData))
		{
			TA_ASSERT_DEV(false, "CommonInventoryActorComponent 초기화에 실패했습니다.");
			return false;
		}

		const CommonInventoryComponentData* component = static_cast<const CommonInventoryComponentData*>(componentData);

#ifndef TA_SERVER
		TAGameEventInitializeInventory* event = new TAGameEventInitializeInventory;
		event->setActorKey(getOwnerActorKey());
		event->setCapacity(component->_capacity);
		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		}
#endif
		return true;
	}
}

