#pragma once

#include "Common/CommonBase.h"
#include "Common/ItemDetail.h"


namespace ta
{
	class ItemGameData;
}


namespace ta
{
	class Item
	{
	public:

		static Item* createItem(const ItemGameData* data, const int32 stackCount) noexcept;
		static Item* createItem(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount) noexcept;

		static Item* copyItems_(const Item* targetItem, const int32 stackCount) noexcept;
		// count에 2를 넣으면 총 5개가 있을때 2개 3개로 나뉜다.
		static Item* divideItemStackCount_(Item*& targetItem, const int32 count) noexcept;

		static Item InvalidItem;
		
		virtual ~Item(void) noexcept; 		

		bool initializeItem_(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount) noexcept;

		bool checkValid_(void) const noexcept;

		void clear_(void) noexcept;

		int32 getStackCount_(void) const noexcept;
		bool setStackCount_(const int32 stackCount) noexcept;

		const ItemGameData* getBase_(void) const noexcept;
		const ItemDetail& getDetail_(void) const noexcept;

	private:
		Item(void) noexcept;
		explicit Item(const ItemGameData* data, const ItemDetail& detail, const int32 stackCount = 1) noexcept;
		explicit Item(const Item& item) noexcept;

		static bool generateRandomItemDetail(const ItemGameData* data, ItemDetail& detail) noexcept;

	private:
		bool _isValid;
		const ItemGameData* _base;
		ItemDetail _detail;

		int32 _stackCount;
	};

}