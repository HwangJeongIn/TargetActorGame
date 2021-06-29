#include "Common/Sectors.h"
#include "Common/KeyDefinition.h"
#include "Common/Sector.h"


namespace ta
{
	Sectors::Sectors(void) noexcept
		: _allSectors(nullptr)
	{
	}

	Sectors::~Sectors(void) noexcept
	{
	}

	bool Sectors::initialize(void) noexcept
	{
		return true;
	}

	bool Sectors::doInitialize(void) noexcept
	{
		Sector* currentSector = nullptr;
		for (uint32 index = 0; index < CountOfSectors; ++index)
		{
			currentSector = getSector(index);
			//if (nullptr == currentSector)
			//{
			//	TA_ASSERT_DEV(false, "비정상입니다.");
			//	return false;
			//}

			if (false == currentSector->initialize())
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}

	bool Sectors::open(void) noexcept
	{
		Sector* currentSector = nullptr;
		for (uint32 index = 0; index < CountOfSectors; ++index)
		{
			currentSector = getSector(index);
			//if (nullptr == currentSector)
			//{
			//	TA_ASSERT_DEV(false, "비정상입니다.");
			//	return false;
			//}

			currentSector->setSectorKey(index);
			if (false == currentSector->open())
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		return true;
	}

	void Sectors::close(void) noexcept
	{
		Sector* currentSector = nullptr;
		for (uint32 index = 0; index < CountOfSectors; ++index)
		{
			currentSector = getSector(index);
			//if (nullptr == currentSector)
			//{
			//	TA_ASSERT_DEV(false, "비정상입니다.");
			//	return false;
			//}

			currentSector->close();
		}

		delete[] _allSectors;
	}

	bool Sectors::checkSectorValid(const SectorKey& sectorKey) const noexcept
	{
		if (false == sectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const int32 index = sectorKey.getKeyValue();
		if (CountOfSectors <= index)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}
}
