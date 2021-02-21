#pragma once

#include "Common/ActorComponentTypeList.h"
#include "Common/ActorComponentTypeList.h"

namespace ta
{
	const std::vector<ActorComponentType> OwnerActor{};

	const std::vector<ActorComponentType> DefaultActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character };

	const std::vector<ActorComponentType> PlayerActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character };

	const std::vector<ActorComponentType> MonsterActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	const std::vector<ActorComponentType> NpcActor{ ActorComponentType::Move
		, ActorComponentType::Action, ActorComponentType::Character, ActorComponentType::Ai };

	//static ActorComponentType DefaultActorSet[] = { ActorComponentType::Move, ActorComponentType::Action };
	//std::vector<ActorComponentType> DefaultActor(DefaultActorSet, DefaultActorSet + sizeof(DefaultActorSet) / sizeof(DefaultActorSet[0]));

	const std::unordered_map<ActorType, const std::vector<ActorComponentType>&> ActorComponentGroups
	{
		{ActorType::Owner, OwnerActor}
		, {ActorType::Default, DefaultActor}
		, {ActorType::Player, PlayerActor}
		, {ActorType::Monster, MonsterActor}
		, {ActorType::Npc, NpcActor}
	};
}

