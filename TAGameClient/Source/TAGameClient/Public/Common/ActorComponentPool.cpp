#include "Common/ActorComponent.h"
#include "Common/ActorComponentPool.h"


namespace ta
{
	ActorComponentPool::ActorComponentPool(const ActorComponentType& actorComponentType) noexcept
		: _actorComponents(nullptr)
		, _actorComponentType(actorComponentType)
	{
		clear();
	}

	ActorComponentPool::~ActorComponentPool(void) noexcept
	{
	}


	bool ActorComponentPool::addComponentCountFromActorType(const ActorType& ownerActorType, const uint32& count) noexcept
	{
		std::pair<std::unordered_map<ActorType, uint32>::const_iterator, bool> rv = _startIndexMap.insert(std::make_pair(ownerActorType, _count));
		if (false == rv.second)
		{
			TA_ASSERT_DEV(false, "2번 추가하려 합니다. ActorType : %d", static_cast<uint8>(ownerActorType));
			return false;
		}

		_count += count;
		if (MaxActorDataPoolCapacity < _count)
		{
			TA_ASSERT_DEV(false, "액터수보다 컴포넌트가 많습니다. 코드버그");
			return false;
		}

		return true;
	}
	
	void ActorComponentPool::clear(void) noexcept
	{
		if (nullptr != _actorComponents)
		{
			delete[] _actorComponents;
			_actorComponents = nullptr;
		}

		_isValid = false;
		_count = 0;
		_startIndexMap.clear();
	}
}

