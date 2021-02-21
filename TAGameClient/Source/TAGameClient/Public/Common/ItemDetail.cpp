#include "Common/ItemDetail.h"


namespace ta
{
	ItemDetail::ItemDetail(void) noexcept
	{
		clear_();
	}

	ItemDetail::ItemDetail(const ItemDetail& data) noexcept
		: _durability(data._durability)
		, _maxDurability(data._maxDurability)
	{
	}

	ItemDetail::~ItemDetail(void) noexcept
	{
	}

	ItemDetail& ItemDetail::operator=(const ItemDetail& data) noexcept
	{
		_durability = (data._durability);
		_maxDurability = (data._maxDurability);
		return *this;
	}

	void ItemDetail::clear_(void) noexcept
	{
		_durability = 0;
		_maxDurability = 0;
	}
}
