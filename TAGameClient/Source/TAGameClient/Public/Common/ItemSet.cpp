#include "Common/ItemSet.h"
#include "Common/Item.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	ItemSet::ItemSet(const uint32 capacity, const ItemSetType itemSetType) noexcept
		: _capacity(capacity)
		, _itemSetType(itemSetType)
	{
		_items.resize(_capacity);
	}

	ItemSet::~ItemSet(void) noexcept
	{
	}

	bool ItemSet::initializeItemSet(const std::vector<ItemElementData> ItemElementDataSet) noexcept
	{
		const uint32 ItemElementDataSetCount = ItemElementDataSet.size();
		if (_capacity != ItemElementDataSetCount)
		{
			TA_ASSERT_DEV(false, "타입갯수와 슬롯갯수가 다릅니다.");
			return false;
		}
		
		for (uint32 index = 0; index < ItemElementDataSetCount; ++index)
		{
			// 슬롯에 어떤 아이템이 들어갈 수 있는지
			_items[index]._itemType = ItemElementDataSet[index]._itemType;

			// 비어있는경우
			if (false == ItemElementDataSet[index]._baseKey.isValid())
			{
				continue;
			}

			const ItemGameData* base = GetGameData<ItemGameData>(ItemElementDataSet[index]._baseKey);
			if (nullptr == base)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			if (false == _items[index]._item->initializeItems(base
				, ItemElementDataSet[index]._detail
				, ItemElementDataSet[index]._stackCount))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}

	void ItemSet::clear_(void) noexcept
	{
		const uint32 count = _items.size();
		for (uint32 index = 0; index < count; ++index)
		{
			_items[index].clear_();
		}
	}

	bool ItemSet::checkCanPop_(const ItemSlotNo slotNo, const int32 stackCount) const noexcept
	{
		if (stackCount <= 0)
		{
			TA_ASSERT_DEV(false, "갯수가 비정상입니다.");
			return false;
		}

		if (false == checkSlotFull_(slotNo))
		{
			TA_ASSERT_DEV(false, "슬롯이 비어있습니다.");
			return false;
		}

		const int32 currentStackCount = _items[slotNo]._item->getStackCount_();
		if (currentStackCount < stackCount)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	Item* ItemSet::popItem_(const ItemSlotNo slotNo, const int32 stackCount) noexcept
	{
		if (false == checkCanPop_(slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return &Item::invalidItem;
		}

		Item* rv = Item::divideItemStackCount_(_items[slotNo]._item, stackCount);
		if (false == rv->checkValid_())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}

		return rv;
	}

	bool ItemSet::checkCanPush_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) const noexcept
	{
		if (stackCount <= 0)
		{
			TA_ASSERT_DEV(false, "갯수가 비정상입니다.");
			return false;
		}

		if (true == checkSlotFull_(slotNo))
		{
			TA_ASSERT_DEV(false, "슬롯이 비어있지 않습니다.");
			return false;
		}

		if (ItemSetType::ContainerType != _itemSetType)
		{ // 모든것을 담을 수 있는 컨테이너 타입이 아닐때 해당 슬롯에 들어가는게 맞는지 확인
			if (_items[slotNo]._itemType != data->getItemType())
			{
				TA_ASSERT_DEV(false, "슬롯에 들어갈 아이템 타입이 맞지 않습니다.");
				return false;
			}
		}

		return true;
	}

	bool ItemSet::pushItem_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) noexcept
	{
		if (false == checkCanPush_(data, slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		Item* newItems = Item::createItems(data, stackCount);
		if (false == newItems->checkValid_())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		_items[slotNo]._item = newItems;
		return true;
	}

	bool ItemSet::checkSlotFull_(const ItemSlotNo slotNo) const noexcept
	{
		if (_items.size() < slotNo)
		{
			TA_ASSERT_DEV(false, "슬롯넘버가 비정상입니다.");
			return false;
		}

		return _items[slotNo]._item->checkValid_();
	}

	const uint32 ItemSet::getCapacity_(void) const noexcept
	{
		return _capacity;
	}
	
	const ItemSetType ItemSet::getItemSetType_(void) const noexcept
	{
		return _itemSetType;
	}

	const std::vector<ItemElement>& ItemSet::getItems_(void) const noexcept
	{
		return _items;
	}
}


namespace ta
{
	ItemElement::ItemElement(void) noexcept
		: _item(&Item::invalidItem)
		, _itemType(ItemType::Count)
	{
	}

	ItemElement::~ItemElement(void) noexcept
	{
	}
	
	void ItemElement::clear_(void) noexcept
	{
		if (true == _item->checkValid_())
		{
			delete _item;
			_item = &(Item::invalidItem);
		}
	}
}