#include "Common/ItemSet.h"
#include "Common/Item.h"
#include "Common/GameData.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	ItemSet::ItemSet(void) noexcept
	{
	}

	ItemSet::~ItemSet(void) noexcept
	{
	}

	bool ItemSet::initializeItemSet(const uint32 capacity, const ItemSetType itemSetType, const ItemElementData* itemElementDataSet) noexcept
	{
		_capacity = capacity;
		_items.resize(_capacity);
		_itemSetType = itemSetType;

		for (uint32 index = 0; index < _capacity; ++index)
		{
			// 비어있는경우
			if (false == itemElementDataSet[index]._baseKey.isValid())
			{
				_items[index] = &Item::InvalidItem;
				continue;
			}

			const ItemGameData* base = GetGameData<ItemGameData>(itemElementDataSet[index]._baseKey);
			if (nullptr == base)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			Item* newItem = Item::createItem(base
											 , itemElementDataSet[index]._detail
											 , itemElementDataSet[index]._stackCount);

			if (nullptr == newItem)
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			_items[index] = newItem;
		}

		return true;
	}

	void ItemSet::clear_(void) noexcept
	{
		const uint32 count = _items.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (true == _items[index]->checkValid_())
			{
				delete _items[index];
			}

			_items[index] = &Item::InvalidItem;
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

		const int32 currentStackCount = _items[slotNo]->getStackCount_();
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
			return &Item::InvalidItem;
		}

		Item* rv = Item::divideItemStackCount_(_items[slotNo], stackCount);
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
			if (_items[slotNo]->getBase_()->getItemType() != data->getItemType())
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

		Item* newItems = Item::createItem(data, stackCount);
		if (false == newItems->checkValid_())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		_items[slotNo] = newItems;
		return true;
	}

	bool ItemSet::checkSlotFull_(const ItemSlotNo slotNo) const noexcept
	{
		if (_items.size() < slotNo)
		{
			TA_ASSERT_DEV(false, "슬롯넘버가 비정상입니다.");
			return false;
		}

		return _items[slotNo]->checkValid_();
	}

	const uint32 ItemSet::getCapacity_(void) const noexcept
	{
		return _capacity;
	}
	
	const ItemSetType ItemSet::getItemSetType_(void) const noexcept
	{
		return _itemSetType;
	}

	const std::vector<Item*>& ItemSet::getItems_(void) const noexcept
	{
		return _items;
	}
}