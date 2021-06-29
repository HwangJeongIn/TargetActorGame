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
		virtual ~ServerSector(void) noexcept override final;

	private:
		// SectorEvent / SectorProperty 등을 처리
		SectorProcessor* _sectorProcessor;
	};
}