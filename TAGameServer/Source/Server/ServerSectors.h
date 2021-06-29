#pragma once
#include "Common/Sectors.h"



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

	};
}


