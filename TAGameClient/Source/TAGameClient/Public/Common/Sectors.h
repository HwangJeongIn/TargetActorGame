#pragma once
#include <unordered_map>
#include <filesystem>
#include "Common/CommonDataTypeDefinition.h"
#include "Common/KeyDefinition.h"


namespace fs = std::filesystem;


namespace ta
{
#if defined(TA_SERVER) && !defined(TA_CLIENT_IN_SERVER)
	extern const fs::path SectorDataFilePath;
#elif !defined(TA_SERVER)
	extern fs::path SectorDataFilePath;
#endif
}


namespace ta
{
	class Sector;
	class SectorKey;
}


namespace ta
{
	class SectorZoneMappingData
	{
	public:
		SectorZoneMappingData(void) noexcept;
		virtual ~SectorZoneMappingData(void) noexcept;

		void setData(const SectorKey& sectorKey, const SectorZoneType& sectorZoneType) noexcept;

	public:
		SectorKey _sectorKey;
		SectorZoneType _sectorZoneType;
	};
}


namespace ta
{
	class Sectors
	{
	public:
		Sectors(void) noexcept;
		virtual ~Sectors(void) noexcept;

		virtual bool initialize(void) noexcept;
		bool doInitialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		virtual Sector* getSector(const SectorKey& sectorKey) noexcept = 0;
		virtual bool loadSectorEventSetDataFromXml(const fs::path& filePath, SectorZoneMappingData* sectorZoneMappingData) noexcept;

	protected:
		bool checkSectorValid(const SectorKey& sectorKey) const noexcept;

	protected:
		Sector* _allSectors;
	};
}


