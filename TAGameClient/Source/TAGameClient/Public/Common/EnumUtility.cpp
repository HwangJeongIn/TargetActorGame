#include "Common/EnumUtility.h"

namespace ta
{
	const std::unordered_map<std::string, ActorType> ActorTypeConverter
	{
		 {"Default", ActorType::Default}
		, {"Owner", ActorType::Owner}
		, {"Player", ActorType::Player}
		, {"ControlledByPlayer", ActorType::ControlledByPlayer}
		, {"Monster", ActorType::Monster}
		, {"Npc", ActorType::Npc}
	};
	TA_COMPILE_DEV(6 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, GameDataType> GameDataTypeConverter
	{
		 {"GroupGameData", GameDataType::GroupGameData}
		, {"MoveGameData", GameDataType::MoveGameData}
		, {"AiGameData", GameDataType::AiGameData}
		, {"CharacterGameData", GameDataType::CharacterGameData}
		, {"ItemGameData", GameDataType::ItemGameData}
	};
	TA_COMPILE_DEV(5 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, AiClassType> AiClassTypeConverter
	{
		 {"Player", AiClassType::Player}
		, {"Monster", AiClassType::Monster}
		, {"Npc", AiClassType::Npc}
	};
	TA_COMPILE_DEV(3 == static_cast<uint8>(AiClassType::Count), "여기도 확인해주세요");
	

	const std::unordered_map<std::string, GameWorldType> GameWorldTypeConverter
	{
		 {"RealWorld", GameWorldType::RealWorld}
		, {"TempWorld", GameWorldType::TempWorld}
	};
	TA_COMPILE_DEV(2 == static_cast<uint8>(GameWorldType::Count), "여기도 확인해주세요");



#define CONVERT_FUNCTION_DEFINITION(EnumTypeName)									 \
																					 \
	template <>																		 \
	bool ConvertEnumToString(EnumTypeName& enumValue, std::string& output) noexcept	 \
	{																				 \
		auto it = EnumTypeName##Converter.begin();									 \
		const auto end = EnumTypeName##Converter.end();								 \
		for (; it != end; ++it)														 \
		{																			 \
			if (enumValue == it->second)											 \
			{																		 \
				output = it->first;													 \
				return true;														 \
			}																		 \
		}																			 \
		return false;																 \
	}																				 \
																					 \
	template <>																		 \
	EnumTypeName ConvertStringToEnum(const std::string& input) noexcept				 \
	{																				 \
		auto target = EnumTypeName##Converter.find(input);							 \
		if (EnumTypeName##Converter.end() == target)								 \
		{																			 \
			TA_ASSERT_DEV(false, "비정상입니다.");									 \
			return EnumTypeName::Count;												 \
		}																			 \
																					 \
		return target->second;														 \
	}																				 \


	CONVERT_FUNCTION_DEFINITION(ActorType)
	CONVERT_FUNCTION_DEFINITION(GameDataType)
	CONVERT_FUNCTION_DEFINITION(AiClassType)
	CONVERT_FUNCTION_DEFINITION(GameWorldType)


#undef CONVERT_FUNCTION_DEFINITION
}																					 



