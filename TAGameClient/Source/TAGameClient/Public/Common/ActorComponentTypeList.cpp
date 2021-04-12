#pragma once

#include "Common/ActorComponentTypeList.h"
#include "Common/CommonBase.h"

namespace ta
{ 
	ActorComponentGroupData::ActorComponentGroupData(const std::vector<ActorComponentType>& componentTypeList, const uint32& countPerComponent) noexcept
		: _componentTypeList(componentTypeList)
		, _countPerComponent(countPerComponent)
	{
	}
}


namespace ta
{
	const std::vector<ActorComponentType> OwnerActor{};

	const std::vector<ActorComponentType> DefaultActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character };

	const std::vector<ActorComponentType> PlayerActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Inventory };

	//const std::vector<ActorComponentType> MonsterActor{ ActorComponentType::Move
	//	, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	const std::vector<ActorComponentType> NpcActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	const std::vector<ActorComponentType> ObjectActor{ ActorComponentType::Character };

	//static ActorComponentType DefaultActorSet[] = { ActorComponentType::Move, ActorComponentType::Action };
	//std::vector<ActorComponentType> DefaultActor(DefaultActorSet, DefaultActorSet + sizeof(DefaultActorSet) / sizeof(DefaultActorSet[0]));



	const std::unordered_map<ActorType, ActorComponentGroupData> ActorComponentGroups
	{
		//{ActorType::Owner, ActorComponentGroupData(OwnerActor, 1)}
		//, {ActorType::Default, DefaultActor}
		, {ActorType::Player, ActorComponentGroupData(PlayerActor, MaxPlayerActorDataPoolCapacity)}
		//, {ActorType::Monster, MonsterActor}
		, {ActorType::Npc, ActorComponentGroupData(NpcActor, MaxNpcActorDataPoolCapacity)}
		, {ActorType::Object, ActorComponentGroupData(ObjectActor, MaxObjectActorDataPoolCapacity)}
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 추가해주세요");

}
