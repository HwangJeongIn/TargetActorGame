#include "Client/ClientInventoryActorSystem.h"
#include "Client/AllPacketClient.h"
#include "Common/CommonInventoryActorComponent.h"
#include "Common/ItemSet.h"
#include "Common/Item.h"
#include "Common/ScopedLock.h"

#ifndef TA_SERVER
#include "TAGameEvent.h"
#endif

namespace ta
{
	ClientInventoryActorSystem::ClientInventoryActorSystem(void) noexcept
	{
	}

	ClientInventoryActorSystem::~ClientInventoryActorSystem(void) noexcept
	{
	}

	void ClientInventoryActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	bool ClientInventoryActorSystem::requestDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo slotNo, const int32 stackCount) const noexcept
	{
		ta::ScopedLock inventoryLock(inventoryCom);
		if (false == inventoryCom->getInventory_()->checkCanPop_(slotNo, stackCount))
		{
			return false;
		}

		SendDropItemCTS(inventoryCom->getOwnerActorKey(), slotNo, stackCount);
		return true;
	}

	bool ClientInventoryActorSystem::requestUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo slotNo, const int32 stackCount) const noexcept
	{
		ta::ScopedLock inventoryLock(inventoryCom);
		if (false == inventoryCom->getInventory_()->checkCanPop_(slotNo, stackCount))
		{
			return false;
		}

		SendUseItemCTS(inventoryCom->getOwnerActorKey(), slotNo, stackCount);
		return true;
	}

	bool ClientInventoryActorSystem::respondDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept
	{
		ScopedLock inventoryLock(inventoryCom);
		ItemSet* inventory = inventoryCom->getInventory_();

		Item* popedItem = inventory->popItem_(slotNo, stackCount);
		delete popedItem;

#ifndef TA_SERVER
		TAGameEventRefreshInventory* event = new TAGameEventRefreshInventory;
		event->setActorKey(inventoryCom->getOwnerActorKey());

		if (false == event->setSlotNo(slotNo))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}

		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}
#endif
		return true;
	}

	bool ClientInventoryActorSystem::respondUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept
	{
		ScopedLock inventoryLock(inventoryCom);
		ItemSet* inventory = inventoryCom->getInventory_();

		Item* popedItem = inventory->popItem_(slotNo, stackCount);
		delete popedItem;
		
#ifndef TA_SERVER
		TAGameEventRefreshInventory* event = new TAGameEventRefreshInventory;
		event->setActorKey(inventoryCom->getOwnerActorKey());

		if (false == event->setSlotNo(slotNo))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}

		if (false == RegisterTAGameEvent(event))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
			return false;
		}
#endif
		return true;
	}
}