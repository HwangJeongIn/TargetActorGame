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

		_inventory = new ItemSet(component->_capacity, component->_itemSetType);
		if (false == _inventory->initializeItemSet(component->_itemElementDataSet))
		{
			TA_ASSERT_DEV(false, "ItemSet 초기화에 실패했습니다.");
			return false;
		}

		return true;
	}

	ComponentData* CommonInventoryActorComponent::packData_(void) noexcept
	{
		CommonInventoryComponentData* data = new CommonInventoryComponentData;

		data->_itemSetType = _inventory->getItemSetType_();
		data->_capacity = _inventory->getCapacity_();
		const std::vector<ItemElement>& items = _inventory->getItems_();

		const uint32 itemCount = items.size();
		data->_itemElementDataSet.resize(itemCount);
		for (uint32 index = 0; index < itemCount; ++index)
		{
			data->_itemElementDataSet[index]._itemType = items[index]._itemType;

			if (false == items[index]._item->checkValid_())
			{
				continue;
			}

			data->_itemElementDataSet[index]._baseKey = items[index]._item->getBase_()->_key;
			data->_itemElementDataSet[index]._detail = items[index]._item->getDetail_();
			data->_itemElementDataSet[index]._stackCount = items[index]._item->getStackCount_();
		}

		return data;
	}

	const ActorComponentType CommonInventoryActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Inventory;
	}

	CommonInventoryActorComponent::CommonInventoryActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Inventory)
	{
	}
}

