#include "Server/ServerInventoryActorSystem.h"
#include "Server/AllPacketServer.h"
#include "Common/CommonInventoryActorComponent.h"
#include "Common/ItemSet.h"
#include "Common/Item.h"
#include "Common/ScopedLock.h"


namespace ta
{
	ServerInventoryActorSystem::ServerInventoryActorSystem(void) noexcept
	{
	}

	ServerInventoryActorSystem::~ServerInventoryActorSystem(void) noexcept
	{
	}

	void ServerInventoryActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}
	
	bool ServerInventoryActorSystem::respondDropItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept
	{
		ScopedLock inventoryLock(inventoryCom);
		ItemSet* inventory = inventoryCom->getInventory_();
		if (false == inventory->checkCanPop_(slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		Item* popedItem = inventory->popItem_(slotNo, stackCount);
		delete popedItem;

		SendDropItemSTC(inventoryCom->getOwnerActorKey(), slotNo, stackCount);

		TA_TEMP_DEV("DB업데이트, 바닥에 드롭되는거 해야하면 바뀌면 구현");
	
		return true;
	}

	bool ServerInventoryActorSystem::respondUseItem(CommonInventoryActorComponent* inventoryCom, const ItemSlotNo& slotNo, const int32& stackCount) const noexcept
	{
		ScopedLock inventoryLock(inventoryCom);
		ItemSet* inventory = inventoryCom->getInventory_();
		if (false == inventory->checkCanPop_(slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		Item* popedItem = inventory->popItem_(slotNo, stackCount);
		delete popedItem;

		SendUseItemSTC(inventoryCom->getOwnerActorKey(), slotNo, stackCount);

		TA_TEMP_DEV("DB업데이트, 아이템 효과 캐릭터 적용 구현");

		return true;
	}
}