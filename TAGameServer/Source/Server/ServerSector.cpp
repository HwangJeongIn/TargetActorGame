#include "Server/ServerSector.h"
#include "Common/SectorProcessor.h"

namespace ta
{
	ServerSector::ServerSector(void) noexcept
		: _sectorProcessor(new SectorProcessor(this))
	{
	}

	ServerSector::~ServerSector(void) noexcept
	{
		if (nullptr != _sectorProcessor)
		{
			delete _sectorProcessor;
		}
	}
}
