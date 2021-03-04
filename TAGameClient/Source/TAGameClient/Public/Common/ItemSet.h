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
	class ItemSet
	{
	public:
		ItemSet(void) noexcept;
		virtual ~ItemSet(void) noexcept;

		bool initializeItemSet(const int32 capacity, const ItemSetType itemSetType, const ItemElementData* itemElementDataSet) noexcept;
		void clear_(void) noexcept;

		bool checkCanPop_(const ItemSlotNo slotNo, const int32 stackCount) const noexcept;
		Item* popItem_(const ItemSlotNo slotNo, const int32 stackCount) noexcept;

		bool checkCanPush_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) const noexcept;
		bool pushItem_(const ItemGameData* data, const ItemSlotNo slotNo, const int32 stackCount) noexcept;

		bool checkSlotFull_(const ItemSlotNo slotNo) const noexcept;

		const Item* getItem_(const ItemSlotNo slotNo) const noexcept;

		const int32 getCapacity_(void) const noexcept;
		const ItemSetType getItemSetType_(void) const noexcept;
		const std::vector<Item*>& getItems_(void) const noexcept;
	private:


	private:
		int32 _capacity;
		ItemSetType _itemSetType;

		std::vector<Item*> _items;
	};
}