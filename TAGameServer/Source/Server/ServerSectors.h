#pragma once
#include "Common/Sectors.h"
#include "Common/CommonDataTypeDefinition.h"
#include "Common/KeyDefinition.h"
#include <unordered_map>
#include <vector>


namespace ta
{
	class SectorEventSetData;
	class XmlNode;
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

		// 스레드에서 돌아갈때 레퍼런스로 넘겼더니 정상적으로 output을 받을 수 없었다. 정확한 원인은 모르겠다.
		//virtual bool loadSectorEventSetDataFromXml(const fs::path& filePath, SectorZoneMappingData& sectorZoneMappingData) noexcept override final;
		virtual bool loadSectorEventSetDataFromXml(const fs::path& filePath, SectorZoneMappingData* sectorZoneMappingData) noexcept override final;

	private:

		bool loadSectorZoneEventDataConfig(const fs::path& filePath) noexcept;
		bool loadSectorEventSetData(const XmlNode* sectorZoneElement, SectorEventSetData* output) noexcept;
		
	private:
		std::unordered_map<SectorKey, SectorZoneType> _sectorZoneTypeData;
		std::unordered_map<SectorZoneType, const SectorEventSetData*> _sectorZoneEventDataSetData;

	};
}


