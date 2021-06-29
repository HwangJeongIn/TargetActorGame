#include "Server/ServerSectors.h"
#include "Server/ServerSector.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	ServerSectors::ServerSectors(void) noexcept
	{
	}

	ServerSectors::~ServerSectors(void) noexcept
	{
	}

	bool ServerSectors::initialize(void) noexcept
	{
		if (false == Sectors::initialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}

		_allSectors = new ServerSector[CountOfSectors];

		if (false == doInitialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}
		return true;
	}

	bool ServerSectors::open(void) noexcept
	{
		if (false == Sectors::open())
		{
			TA_ASSERT_DEV(false, "Sectors open에 실패했습니다.");
			return false;
		}

		return true;
	}

	void ServerSectors::close(void) noexcept
	{
		Sectors::close();
	}

	Sector* ServerSectors::getSector(const SectorKey& sectorKey) noexcept
	{
		if (false == checkSectorValid(sectorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return nullptr;
		}

		const int32 index = sectorKey.getKeyValue();
		ServerSector* indexPtr = static_cast<ServerSector*>(_allSectors);

		return &(indexPtr[index]);
	}

}
