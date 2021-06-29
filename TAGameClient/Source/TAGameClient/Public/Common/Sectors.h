#pragma once
#include <unordered_map>
#include <filesystem>


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

	protected:
		bool checkSectorValid(const SectorKey& sectorKey) const noexcept;

	protected:
		Sector* _allSectors;
	};
}


