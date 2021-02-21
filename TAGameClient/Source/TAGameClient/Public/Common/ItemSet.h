#pragma once

#include "Common/CommonBase.h"
#include "Common/ComponentData.h"
#include <vector>


namespace ta
{
	class Item;
	class ItemGameData;
}


namespace ta
{
	class ItemElement
	{
	public:
		ItemElement(void) noexcept;
		virtual ~ItemElement(void) noexcept;

		void clear_(void) noexcept;

		Item* _item;
		ItemType _itemType;
	};
}


namespace ta
{
	class ItemSet
	{
	public:
		explicit ItemSet(const uint32 capacity, const ItemSetType itemSetType) noexcept;
		virtual ~ItemSet(void) noexcept;

		bool initializeItemSet(const std::vector<ItemElementData> ItemElementDataSet) noexcept;
		void clear_(void) noexcept;

		bool checkCanPop_(const ItemSlotNo slotNo, const int32 stackCount) const noexcept;
		Item* popItem_(const ItemSlotNo slotNo, const int32 stackCount) noexcept;

		bool checkCanPush_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) const noexcept;
		bool pushItem_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) noexcept;

		bool checkSlotFull_(const ItemSlotNo slotNo) const noexcept;

		const uint32 getCapacity_(void) const noexcept;
		const ItemSetType getItemSetType_(void) const noexcept;
		const std::vector<ItemElement>& getItems_(void) const noexcept;
	private:


	private:
		const uint32 _capacity;
		const ItemSetType _itemSetType;

		std::vector<ItemElement> _items;
	};
}