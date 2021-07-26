#include "Common/KeyStringUtility.h"
#include <sstream>


namespace ta
{
	TA_COMPILE_DEV(15 == static_cast<uint8>(KeyType::Count), "여기도 추가하세요");


#define STRING_TO_KEY_DEFINITION(KeyName)								  \
	template<> KeyName FromStringCast(const std::string& input) noexcept  \
	{																	  \
		KeyName##Type rv = 0;											  \
		std::istringstream(input.c_str()) >> rv;						  \
		return KeyName(rv);												  \
	}

		STRING_TO_KEY_DEFINITION(ActorKey)
		STRING_TO_KEY_DEFINITION(CharacterDBNoKey)
		STRING_TO_KEY_DEFINITION(SectorKey)
		STRING_TO_KEY_DEFINITION(PathPointPathKey)
		STRING_TO_KEY_DEFINITION(SpawnDataGroupKey)

		STRING_TO_KEY_DEFINITION(GroupGameDataKey)
		STRING_TO_KEY_DEFINITION(MoveGameDataKey)
		STRING_TO_KEY_DEFINITION(AiGameDataKey)
		STRING_TO_KEY_DEFINITION(CharacterGameDataKey)
		STRING_TO_KEY_DEFINITION(ItemGameDataKey)
		STRING_TO_KEY_DEFINITION(RenderingGameDataKey)
		STRING_TO_KEY_DEFINITION(ConditionGameDataKey)
		STRING_TO_KEY_DEFINITION(EventGameDataKey)
		STRING_TO_KEY_DEFINITION(SectorZoneGameDataKey)
		STRING_TO_KEY_DEFINITION(BuffGameDataKey)

#undef STRING_TO_KEY_DEFINITION
}

