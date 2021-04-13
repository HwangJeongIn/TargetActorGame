#pragma once

#include "Common/Sector.h"

// 아이템 만드는 것도 서버쪽이니 서버에서 관리해보자

namespace ta
{
	class ServerSector : public Sector
	{
	public:
		ServerSector(void) noexcept;
		~ServerSector(void) noexcept;
	};
}