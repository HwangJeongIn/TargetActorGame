#pragma once

#include "Common/CommonDataTypeDefinition.h"
#include <vector>
#include <unordered_map>


namespace ta
{
	class ActorGroup
	{
	public:
		ActorGroup(const std::vector<ActorComponentType>& componentTypeList, const uint32& countPerComponent) noexcept;

	public:
		const std::vector<ActorComponentType>& _componentTypeList;
		const uint32 _count;
	};
}


namespace ta
{
	extern uint32 MaxActorDataPoolCapacity;
}


namespace ta
{
	//extern const std::vector<ActorComponentType> OwnerActor;
	//extern const std::vector<ActorComponentType> DefaultActor;
	//extern const std::vector<ActorComponentType> PlayerActor;
	//extern const std::vector<ActorComponentType> MonsterActor;
	//extern const std::vector<ActorComponentType> NpcActor;
	extern const std::unordered_map<ActorType, ActorGroup> ActorDataGroups;
}

