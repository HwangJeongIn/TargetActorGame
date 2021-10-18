#pragma once
#include "Common/KeyDefinition.h"


namespace ta
{
	class ContentParameter
	{
	public:
		explicit ContentParameter(const ActorKey& myActorKey = ActorKey()
								  , const ActorKey& targetActorKey = ActorKey()
								  , const SectorKey& sectorKey = SectorKey()) noexcept;

		explicit ContentParameter(const SectorKey& sectorKey) noexcept;

	public:
		ActorKey _myActorKey;
		ActorKey _targetActorKey;
		SectorKey _sectorKey;
	};
}