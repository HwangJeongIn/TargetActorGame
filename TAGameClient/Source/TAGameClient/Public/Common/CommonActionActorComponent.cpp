#include "Common/CommonActionActorComponent.h"
#include "Common/ComponentData.h"

namespace ta
{
	CommonActionActorComponent::~CommonActionActorComponent(void) noexcept
	{
	}

	bool CommonActionActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		const CommonActionComponentData* component = static_cast<const CommonActionComponentData*>(componentData);

		return true;
	}

	ComponentData* CommonActionActorComponent::packData_(void) noexcept
	{
		CommonActionComponentData* data = new CommonActionComponentData;
		
		// Common초기화

		return data;
	}

	const ActorComponentType CommonActionActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Action;
	}

	CommonActionActorComponent::CommonActionActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Action)
	{
	}
}

