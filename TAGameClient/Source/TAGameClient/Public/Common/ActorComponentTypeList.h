#pragma once

#include "Common/CommonDataTypeDefinition.h"
#include <vector>
#include <unordered_map>

namespace ta
{
	extern const std::vector<ActorComponentType> OwnerActor;
	extern const std::vector<ActorComponentType> DefaultActor;
	extern const std::vector<ActorComponentType> PlayerActor;
	extern const std::vector<ActorComponentType> MonsterActor;
	extern const std::vector<ActorComponentType> NpcActor;
	extern const std::unordered_map<ActorType, const std::vector<ActorComponentType>&> ActorComponentGroups;
}

