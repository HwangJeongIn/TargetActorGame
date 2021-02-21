#include "Server/ServerActorDataPool.h"
#include "Server/ServerActor.h"
#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerActionActorComponent.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/ServerCharacterActorComponent.h"
#include "Server/ServerInventoryActorComponent.h"


namespace ta
{
	ServerActorDataPool::ServerActorDataPool(void) noexcept
	{
	}

	ServerActorDataPool::~ServerActorDataPool(void) noexcept
	{
	}

	bool ServerActorDataPool::initialize(void) noexcept
	{
		if (false == __super::initialize())
		{
			TA_ASSERT_DEV(false, "ActorDataPool initialize에 실패했습니다.");
			return false;
		}

		_actorPoolValues = new ServerActor[_maxCount];
		_moveComponentPoolValues = new ServerMoveActorComponent[_maxCount];
		_actionComponentPoolValues = new ServerActionActorComponent[_maxCount];
		_aiComponentPoolValues = new ServerAiActorComponent[_maxCount];
		_characterComponentPoolValues = new ServerCharacterActorComponent[_maxCount];
		_inventoryComponentPoolValues = new ServerInventoryActorComponent[_maxCount];

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		return true;
	}

	bool ServerActorDataPool::open(void) noexcept
	{
		if (false == __super::open())
		{
			TA_ASSERT_DEV(false, "ActorDataPool open에 실패했습니다.");
			return false;
		}

		return true;
	}

	void ServerActorDataPool::close(void) noexcept
	{
		__super::close();
	}
	
	CommonActor* ServerActorDataPool::getActor(const ActorKey& actorKey) noexcept
	{
		const uint32 index = actorKey.getKeyValue();
		ServerActor* indexPtr = static_cast<ServerActor*>(_actorPoolValues);
		return &indexPtr[index];
	}

	ActorComponent* ServerActorDataPool::getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept
	{
		const uint32 index = actorKey.getKeyValue();

		switch (componentType)
		{
#define RETURN_COMPONENTS(Type, PoolName)																								\
		case ActorComponentType::Type:																									\
			{																															\
				Server##Type##ActorComponent* indexPtr = static_cast<Server##Type##ActorComponent*>(PoolName);							\
				return &indexPtr[index];																								\
			}																															\
			break;																														\


			RETURN_COMPONENTS(Move,_moveComponentPoolValues)
			RETURN_COMPONENTS(Action, _actionComponentPoolValues)
			RETURN_COMPONENTS(Ai, _aiComponentPoolValues)
			RETURN_COMPONENTS(Character, _characterComponentPoolValues)
			RETURN_COMPONENTS(Inventory, _inventoryComponentPoolValues)

#undef RETURN_COMPONENTS

		case ActorComponentType::Count:
				{
					TA_ASSERT_DEV(false, "다른 타입이 들어왔습니다.");
				}
		default:
				{
					TA_ASSERT_DEV(false, "다른 타입이 들어왔습니다.");
				}

		}

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		return nullptr;
	}
}
