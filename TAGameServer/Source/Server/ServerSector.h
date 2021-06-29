#pragma once

#include "Common/Sector.h"


namespace ta
{
	class SectorProcessor;
}

namespace ta
{
	class ServerSector : public Sector
	{
	public:
		ServerSector(void) noexcept;
		~ServerSector(void) noexcept;

	private:
		// SectorEvent / SectorProperty 등을 처리
		SectorProcessor* _sectorProcessor;
	};
}