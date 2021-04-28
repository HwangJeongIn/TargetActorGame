#pragma once
#include "Common/CommonBase.h"


namespace ta
{
	class Sector;
}


namespace ta
{
	class SectorProcessor
	{
	public:
		SectorProcessor(const Sector* owner) noexcept;
		virtual ~SectorProcessor(void) noexcept;

		const Sector* getOwner(void) const noexcept;

	private:
		const Sector* _owner;
	};
}
