#include "Common/CommonObjectActorComponent.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/GameData.h"


namespace ta
{
	CommonObjectActorComponent::CommonObjectActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Object)
		, _itemGameData(nullptr)
		, _renderingGameData(nullptr)
	{
	}

	CommonObjectActorComponent::~CommonObjectActorComponent(void) noexcept
	{
	}

	bool CommonObjectActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		// 이런경우(데이터를 이용해서 이동)도 있지만 보통 내부 데이터를 초기화한다.
		const CommonObjectComponentData* data = static_cast<const CommonObjectComponentData*>(componentData);
		if (true == data->_itemGameDataKey.isValid())
		{
			_itemGameData = GetGameData<ItemGameData>(data->_itemGameDataKey);
			if (nullptr == _itemGameData)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
			_renderingGameData = nullptr;
		}
		else
		{
			if (false == data->_renderingGameDataKey.isValid())
			{
				TA_ASSERT_DEV(false, "둘중 하나는 유효해야합니다.");
				return false;
			}

			_renderingGameData = GetGameData<RenderingGameData>(data->_renderingGameDataKey);
			if (nullptr == _renderingGameData)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
			_itemGameData = nullptr;
		}

		return true;
	}

	ComponentData* CommonObjectActorComponent::packData_(void) noexcept
	{
		CommonObjectComponentData* data = new CommonObjectComponentData;
		if (nullptr != _itemGameData)
		{
			data->_itemGameDataKey = _itemGameData->_key;
		}
		else
		{
			if (nullptr == _renderingGameData)
			{
				TA_ASSERT_DEV(false, "둘중 하나는 유효해야합니다.");
				return nullptr;
			}

			data->_renderingGameDataKey = _renderingGameData->_key;
		}
		
		return data;
	}

	const ActorComponentType CommonObjectActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Object;
	}
}

