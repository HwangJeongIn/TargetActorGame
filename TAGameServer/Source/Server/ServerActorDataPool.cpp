#include "Common/ActorComponentPool.h"
#include "Common/ActorDataGroups.h"
#include "Server/ServerActorDataPool.h"
#include "Server/ServerActor.h"
#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerActionActorComponent.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/ServerCharacterActorComponent.h"
#include "Server/ServerInventoryActorComponent.h"
#include "Server/ServerObjectActorComponent.h"


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
		if (false == ActorDataPool::initialize())
		{
			TA_ASSERT_DEV(false, "ActorDataPool initialize에 실패했습니다.");
			return false;
		}

		_actorPoolValues = new ServerActor[MaxActorDataPoolCapacity];
		if (false == _moveComponentPool->allocatePoolFromInitializedCount<ServerMoveActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _actionComponentPool->allocatePoolFromInitializedCount<ServerActionActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _aiComponentPool->allocatePoolFromInitializedCount<ServerAiActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _characterComponentPool->allocatePoolFromInitializedCount<ServerCharacterActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _inventoryComponentPool->allocatePoolFromInitializedCount<ServerInventoryActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _objectComponentPool->allocatePoolFromInitializedCount<ServerObjectActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

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
		ActorType actorType = ActorType::Count;
		uint32 relativeGroupIndex = 0;
		if (false == getRelativeGroupIndexAndActorType(actorKey, actorType, relativeGroupIndex))
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return nullptr;
		}

		switch (componentType)
		{
#define RETURN_COMPONENTS(Type, PoolName)																			\
		case ActorComponentType::Type:																				\
			{																										\
				return PoolName->getActorComponent<Server##Type##ActorComponent>(actorType, relativeGroupIndex);	\
			}																										\
			break;																									\


			RETURN_COMPONENTS(Move, _moveComponentPool)
			RETURN_COMPONENTS(Action, _actionComponentPool)
			RETURN_COMPONENTS(Ai, _aiComponentPool)
			RETURN_COMPONENTS(Character, _characterComponentPool)
			RETURN_COMPONENTS(Inventory, _inventoryComponentPool)
			RETURN_COMPONENTS(Object, _objectComponentPool)

#undef RETURN_COMPONENTS

		case ActorComponentType::Count:
		default:
				{
					TA_ASSERT_DEV(false, "다른 타입이 들어왔습니다.");
				}
		}

		TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		return nullptr;
	}
}
