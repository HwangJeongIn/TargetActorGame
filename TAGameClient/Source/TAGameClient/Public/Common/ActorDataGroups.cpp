#pragma once

#include "Common/ActorDataGroups.h"
#include "Common/CommonBase.h"

namespace ta
{ 
	ActorGroup::ActorGroup(const std::vector<ActorComponentType>& componentTypeList, const uint32& countPerComponent) noexcept
		: _componentTypeList(componentTypeList)
		, _count(countPerComponent)
	{
	}
}


namespace ta
{
	uint32 MaxPlayerActorDataPoolCapacity = 200;
	uint32 MaxNpcActorDataPoolCapacity = 200;
	uint32 MaxObjectActorDataPoolCapacity = 100;

	uint32 MaxActorDataPoolCapacity = MaxPlayerActorDataPoolCapacity + MaxNpcActorDataPoolCapacity + MaxObjectActorDataPoolCapacity;

	TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");
}


namespace ta
{
	//const std::vector<ActorComponentType> OwnerActorComponentList{};

	//const std::vector<ActorComponentType> MonsterActor{ ActorComponentType::Move
	//	, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	//static ActorComponentType DefaultActorSet[] = { ActorComponentType::Move, ActorComponentType::Action };
	//std::vector<ActorComponentType> DefaultActor(DefaultActorSet, DefaultActorSet + sizeof(DefaultActorSet) / sizeof(DefaultActorSet[0]));

	const std::vector<ActorComponentType> PlayerActorComponentList{ ActorComponentType::Move
	, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Inventory };

	const std::vector<ActorComponentType> NpcActorComponentList{ ActorComponentType::Move
	, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	const std::vector<ActorComponentType> ObjectActorComponentList{ ActorComponentType::Character };


	const std::unordered_map<ActorType, ActorGroup> ActorDataGroups
	{
		{ActorType::Player, ActorGroup(PlayerActorComponentList, MaxPlayerActorDataPoolCapacity)}
		, {ActorType::Npc, ActorGroup(NpcActorComponentList, MaxNpcActorDataPoolCapacity)}
		, {ActorType::Object, ActorGroup(ObjectActorComponentList, MaxObjectActorDataPoolCapacity)}
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 추가해주세요");

}
