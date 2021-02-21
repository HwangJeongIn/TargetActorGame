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
		}

		_actorPoolValues = new ClientActor[_maxCount];
		_moveComponentPoolValues = new ClientMoveActorComponent[_maxCount];
		_actionComponentPoolValues = new ClientActionActorComponent[_maxCount];
		_aiComponentPoolValues = new ClientAiActorComponent[_maxCount];
		_characterComponentPoolValues = new ClientCharacterActorComponent[_maxCount];
		_inventoryComponentPoolValues = new ClientInventoryActorComponent[_maxCount];

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

		switch (componentType)
		{
#define RETURN_COMPONENTS(Type, PoolName)																								\
		case ActorComponentType::Type:																									\
			{																															\
				Client##Type##ActorComponent* indexPtr = static_cast<Client##Type##ActorComponent*>(PoolName);						\
				return &indexPtr[index];																								\
			}																															\
			break;																														\


			RETURN_COMPONENTS(Move, _moveComponentPoolValues)
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
