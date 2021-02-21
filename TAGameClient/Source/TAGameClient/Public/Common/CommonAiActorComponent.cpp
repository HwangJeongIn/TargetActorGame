#include "Common/CommonAiActorComponent.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonAiActorSystem.h"


namespace ta
{
	CommonAiActorComponent::~CommonAiActorComponent(void) noexcept
	{
	}

	bool CommonAiActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		// 이런경우(데이터를 이용해서 이동)도 있지만 보통 내부 데이터를 초기화한다.
		//const CommonAiComponentData* data = static_cast<const CommonAiComponentData*>(componentData);

		return true;
	}

	ComponentData* CommonAiActorComponent::packData_(void) noexcept
	{
		CommonMoveComponentData* data = new CommonMoveComponentData;

		// Common초기화
		return data;
	}

	const ActorComponentType CommonAiActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Ai;
	}

	CommonAiActorComponent::CommonAiActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Ai)
		, _attackRange(2.0f)
	{
	}
}

