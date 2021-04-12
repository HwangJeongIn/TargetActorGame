#include "Common/ActorComponentPool.h"
#include "Common/ActorDataGroups.h"
#include "Client/ClientActorDataPool.h"
#include "Client/ClientActor.h"
#include "Client/ClientMoveActorComponent.h"
#include "Client/ClientActionActorComponent.h"
#include "Client/ClientAiActorComponent.h"
#include "Client/ClientCharacterActorComponent.h"
#include "Client/ClientInventoryActorComponent.h"


namespace ta
{
	ClientActorDataPool::ClientActorDataPool(void) noexcept
	{
	}

	ClientActorDataPool::~ClientActorDataPool(void) noexcept
	{
	}

	bool ClientActorDataPool::initialize(void) noexcept
	{
		if (false == ActorDataPool::initialize())
		{
			TA_ASSERT_DEV(false, "ActorDataPool initialize에 실패했습니다.");
			return false;
		}

		_actorPoolValues = new ClientActor[MaxActorDataPoolCapacity];
		if (false == _moveComponentPool->allocatePoolFromInitializedCount<ClientActionActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _actionComponentPool->allocatePoolFromInitializedCount<ClientAiActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _characterComponentPool->allocatePoolFromInitializedCount<ClientCharacterActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (false == _inventoryComponentPool->allocatePoolFromInitializedCount<ClientInventoryActorComponent>())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		return true;
	}

	bool ClientActorDataPool::open(void) noexcept
	{
		if (false == ActorDataPool::open())
		{
			TA_ASSERT_DEV(false, "ActorDataPool open에 실패했습니다.");
			return false;
		}

		return true;
	}

	void ClientActorDataPool::close(void) noexcept
	{
		ActorDataPool::close();
	}

	CommonActor* ClientActorDataPool::getActor(const ActorKey& actorKey) noexcept
	{
		const uint32 index = actorKey.getKeyValue();
		ClientActor* indexPtr = static_cast<ClientActor*>(_actorPoolValues);
		return &indexPtr[index];
	}


	ActorComponent* ClientActorDataPool::getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept
	{
		const uint32 index = actorKey.getKeyValue();

		ActorType actorType = ActorType::Count;
		uint32 relativeGroupIndex = 0;
		if (false == getRelativeGroupIndexAndActorType(actorKey, actorType, relativeGroupIndex))
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return false;
		}
		
		_moveComponentPool->getActorComponent<ClientMoveActorComponent>(actorType, relativeGroupIndex);

		switch (componentType)
		{
#define RETURN_COMPONENTS(Type, PoolName)																								\
		case ActorComponentType::Type:																									\
			{																															\
				Client##Type##ActorComponent* indexPtr = static_cast<Client##Type##ActorComponent*>(PoolName);						\
				return &indexPtr[index];																								\
			}																															\
			break;																														\


			RETURN_COMPONENTS(Move, _moveComponentPool)
			RETURN_COMPONENTS(Action, _actionComponentPool)
			RETURN_COMPONENTS(Ai, _aiComponentPool)
			RETURN_COMPONENTS(Character, _characterComponentPool)
			RETURN_COMPONENTS(Inventory, _inventoryComponentPool)

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
