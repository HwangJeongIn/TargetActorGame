#pragma once


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


