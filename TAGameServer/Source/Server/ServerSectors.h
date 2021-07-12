#pragma once
#include "Common/Sectors.h"
#include "Common/CommonDataTypeDefinition.h"
#include "Common/KeyDefinition.h"
#include <unordered_map>
#include <vector>


namespace ta
{
	class SectorEventData;
}


namespace ta
{
	class ServerSectors : public Sectors
	{
	public:
		ServerSectors(void) noexcept;
		virtual ~ServerSectors(void) noexcept override final;

		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

		virtual Sector* getSector(const SectorKey& sectorKey) noexcept override final;

	private:
		std::unordered_map<SectorKey, SectorZoneType> _sectorZoneData;
		std::unordered_map<SectorZoneType, std::vector<SectorEventData*>> _sectorZoneEventData;

	};
}


