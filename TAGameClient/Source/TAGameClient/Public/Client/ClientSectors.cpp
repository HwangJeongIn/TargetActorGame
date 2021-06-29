#include "Client/ClientSectors.h"
#include "Client/ClientSector.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	ClientSectors::ClientSectors(void) noexcept
	{
	}

	ClientSectors::~ClientSectors(void) noexcept
	{
	}

	bool ClientSectors::initialize(void) noexcept
	{
		if (false == Sectors::initialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}

		_allSectors = new ClientSector[CountOfSectors];

		if (false == doInitialize())
		{
			TA_ASSERT_DEV(false, "Sectors initialize에 실패했습니다.");
			return false;
		}
		return true;
	}

	bool ClientSectors::open(void) noexcept
	{
		if (false == Sectors::open())
		{
			TA_ASSERT_DEV(false, "Sectors open에 실패했습니다.");
			return false;
		}

		return true;
	}

	void ClientSectors::close(void) noexcept
	{
		Sectors::close();
	}

	Sector* ClientSectors::getSector(const SectorKey& sectorKey) noexcept
	{
		if (false == checkSectorValid(sectorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return nullptr;
		}

		const int32 index = sectorKey.getKeyValue();
		ClientSector* indexPtr = static_cast<ClientSector*>(_allSectors);

		return &(indexPtr[index]);
	}

}
