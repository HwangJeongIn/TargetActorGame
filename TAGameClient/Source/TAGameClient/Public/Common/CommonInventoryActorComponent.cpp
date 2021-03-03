#include "Common/CommonInventoryActorComponent.h"
#include "Common/ComponentData.h"
#include "Common/ItemSet.h"
#include "Common/Item.h"
#include "Common/GameData.h"


namespace ta
{
	CommonInventoryActorComponent::~CommonInventoryActorComponent(void) noexcept
	{
	}

	bool CommonInventoryActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		const CommonInventoryComponentData* component = static_cast<const CommonInventoryComponentData*>(componentData);

		_inventory = new ItemSet;
		if (false == _inventory->initializeItemSet(component->_capacity, component->_itemSetType, component->_itemElementDataSet))
		{
			TA_ASSERT_DEV(false, "ItemSet 초기화에 실패했습니다.");
			return false;
		}

		return true;
	}

	ComponentData* CommonInventoryActorComponent::packData_(void) noexcept
	{
		CommonInventoryComponentData* data = new CommonInventoryComponentData;

		const uint32 inventoryCapacity = static_cast<uint32>(_inventory->getCapacity_());
		data->_capacity = inventoryCapacity;
		data->_itemSetType = ItemSetType::ContainerType;
		//data->_itemElementDataSet.resize(inventoryCapacity);

		const std::vector<Item*>& items = _inventory->getItems_();
		for (uint32 index = 0; index < inventoryCapacity; ++index)
		{
			if (false == items[index]->checkValid_())
			{
				continue;
			}

			data->_itemElementDataSet[index]._baseKey = items[index]->getBase_()->_key;
			data->_itemElementDataSet[index]._detail = items[index]->getDetail_();
			data->_itemElementDataSet[index]._stackCount = items[index]->getStackCount_();
		}

		return data;
	}

	const ActorComponentType CommonInventoryActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Inventory;
	}

	ItemSet* CommonInventoryActorComponent::getInventory_(void) noexcept
	{
		return _inventory;
	}

	CommonInventoryActorComponent::CommonInventoryActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Inventory)
	{
	}
}

