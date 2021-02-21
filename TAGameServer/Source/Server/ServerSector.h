#pragma once

#include "Common/Sector.h"


namespace ta
{
	class ServerSector : public Sector
	{
	public:
		ServerSector(void) noexcept;
		~ServerSector(void) noexcept;
	};
}