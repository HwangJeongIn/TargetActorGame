#include "Common/CommonActorSystemManager.h"
#include "Common/KeyDefinition.h"
#include "Common/ActorSystem.h"


namespace ta
{
	CommonActorSystemManager::CommonActorSystemManager(CommonActorManager* actorManager) noexcept
		: _actorManager(actorManager)
	{
		// 수정함
		//if (false == initialize())
		//{
		//	TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
		//}

		//if (false == open())
		//{
		//	TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
		//}
	}

	CommonActorSystemManager::~CommonActorSystemManager(void) noexcept
	{
		// 수정함
		//close();
	}

	bool CommonActorSystemManager::initialize(void) noexcept
	{
		return true;
	}

	bool CommonActorSystemManager::open(void) noexcept
	{
		return true;
	}

	void CommonActorSystemManager::close(void) noexcept
	{
		const uint32 count = _actorSystems.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (nullptr != _actorSystems[index])
			{
				delete _actorSystems[index];
			}
		}
		
		_actorSystems.clear();
	}
	
	ActorSystem* CommonActorSystemManager::getActorSystem(const ActorSystemType systemType) noexcept
	{
		uint32 systemIndex = static_cast<uint32>(systemType);
		if (static_cast<uint32>(ActorSystemType::Count) <= systemIndex)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return nullptr;
		}
		return _actorSystems[systemIndex];
	}
}

