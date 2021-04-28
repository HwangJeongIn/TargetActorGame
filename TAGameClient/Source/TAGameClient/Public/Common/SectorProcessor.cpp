#include "Common/SectorProcessor.h"
#include "Common/Sector.h"


namespace ta
{
	SectorProcessor::SectorProcessor(const Sector* owner) noexcept
		: _owner(owner)
	{
		TA_ASSERT_DEV((nullptr != _owner), "비정상");
	}

	SectorProcessor::~SectorProcessor(void) noexcept
	{
	}
	
	const Sector* SectorProcessor::getOwner(void) const noexcept
	{
		return _owner;
	}
}
