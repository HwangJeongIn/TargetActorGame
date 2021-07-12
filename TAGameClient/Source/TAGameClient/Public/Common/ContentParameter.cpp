#include "Common/ContentParameter.h"

namespace ta
{
	ContentParameter::ContentParameter(const ActorKey& myActorKey
																	   , const ActorKey& targetActorKey
																	   , const SectorKey& sectorKey) noexcept
		: _myActorKey(myActorKey)
		, _targetActorKey(targetActorKey)
		, _sectorKey(sectorKey)
	{

	}
}
