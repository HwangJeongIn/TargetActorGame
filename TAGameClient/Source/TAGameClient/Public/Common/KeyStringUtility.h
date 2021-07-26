#pragma once

#include "Common/StringUtility.h"
#include "Common/KeyDefinition.h"
#include <string>


namespace ta
{
	TA_COMPILE_DEV(15 == static_cast<uint8>(KeyType::Count), "여기도 추가하세요");

#define STRING_TO_KEY_DECLARATION(KeyName)									  \
																			  \
	template<> KeyName FromStringCast(const std::string& input) noexcept;	  \

	STRING_TO_KEY_DECLARATION(ActorKey)
	STRING_TO_KEY_DECLARATION(CharacterDBNoKey)
	STRING_TO_KEY_DECLARATION(SectorKey)
	STRING_TO_KEY_DECLARATION(PathPointPathKey)
	STRING_TO_KEY_DECLARATION(SpawnDataGroupKey)

	STRING_TO_KEY_DECLARATION(GroupGameDataKey)
	STRING_TO_KEY_DECLARATION(MoveGameDataKey)
	STRING_TO_KEY_DECLARATION(AiGameDataKey)
	STRING_TO_KEY_DECLARATION(CharacterGameDataKey)
	STRING_TO_KEY_DECLARATION(ItemGameDataKey)
	STRING_TO_KEY_DECLARATION(RenderingGameDataKey)
	STRING_TO_KEY_DECLARATION(ConditionGameDataKey)
	STRING_TO_KEY_DECLARATION(EventGameDataKey)
	STRING_TO_KEY_DECLARATION(SectorZoneGameDataKey)
	STRING_TO_KEY_DECLARATION(BuffGameDataKey)

#undef STRING_TO_KEY_DECLARATION
}