#pragma once

#include "Common/CommonBase.h"


namespace ta
{
	class ItemDetail
	{
	public:
		ItemDetail(void) noexcept;
		ItemDetail(const ItemDetail& data) noexcept;
		
		virtual ~ItemDetail(void) noexcept;

		ItemDetail& operator=(const ItemDetail& data) noexcept;

		void clear_(void) noexcept;

	public:
		// 세부정보
		int32 _durability;
		int32 _maxDurability;
	};
}