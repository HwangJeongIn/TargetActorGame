#pragma once
#include "Common/KeyDefinition.h"


namespace ta
{
	class ContentParameter
	{
	public:
		explicit ContentParameter(const ActorKey& myActorKey
												  , const ActorKey& targetActorKey
												  , const SectorKey& sectorKey) noexcept;

	public:
		ActorKey _myActorKey;
		ActorKey _targetActorKey;
		SectorKey _sectorKey;
	};
}