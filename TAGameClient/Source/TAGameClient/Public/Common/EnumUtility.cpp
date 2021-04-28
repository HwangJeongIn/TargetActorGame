#include "Common/EnumUtility.h"


namespace ta
{
	const std::unordered_map<std::string, ActorType> ActorTypeConverter
	{
		 {"Player", ActorType::Player}
		, {"Npc", ActorType::Npc}
		, {"Object", ActorType::Object}
		, {"ControlledByPlayer", ActorType::ControlledByPlayer}
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, GameDataType> GameDataTypeConverter
	{
		 {"GroupGameData", GameDataType::GroupGameData}
		, {"MoveGameData", GameDataType::MoveGameData}
		, {"AiGameData", GameDataType::AiGameData}
		, {"CharacterGameData", GameDataType::CharacterGameData}
		, {"ItemGameData", GameDataType::ItemGameData}
		, {"RenderingGameData", GameDataType::RenderingGameData}
	};
	TA_COMPILE_DEV(6 == static_cast<uint8>(GameDataType::Count), "여기도 확인해주세요");


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
	

	const std::unordered_map<std::string, ItemType> ItemTypeConverter
	{
		{ "All", ItemType::All }
		,{ "Consumable", ItemType::Consumable }
		,{ "RHand", ItemType::RHand }
		,{ "LHand", ItemType::LHand }
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(ItemType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, TADataType> TADataTypeConverter
	{
		 { "Int8", TADataType::Int8	}
		,{ "Int16", TADataType::Int16 }
		,{ "Int32", TADataType::Int32 }
		,{ "Int64", TADataType::Int64 }
		,{ "Uint8", TADataType::Uint8 }
		,{ "Uint16", TADataType::Uint16 }
		,{ "Uint32", TADataType::Uint32 }
		,{ "Uint64", TADataType::Uint64 }
		,{ "Bool", TADataType::Bool }
		,{ "Float", TADataType::Float }
		,{ "StdString", TADataType::StdString }
		,{ "String", TADataType::String }
	};
	TA_COMPILE_DEV(12 == static_cast<uint8>(TADataType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, InteractionType> InteractionTypeConverter
	{
		 { "Talk", InteractionType::Talk }
		,{ "Greet", InteractionType::Greet }
		,{ "Gather", InteractionType::Gather }
		,{ "Attack", InteractionType::Attack }
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(InteractionType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, MeshType> MeshTypeConverter
	{
		 { "Skeletal", MeshType::Skeletal }
		,{ "Static", MeshType::Static }
	};
	TA_COMPILE_DEV(2 == static_cast<uint8>(MeshType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, SectorZoneType> SectorZoneTypeConverter
	{
		 { "Forest", SectorZoneType::Forest }
		,{ "Sea", SectorZoneType::Sea }
		,{ "Desert", SectorZoneType::Desert }
		,{ "Jungle", SectorZoneType::Jungle }
	};
	TA_COMPILE_DEV(4 == static_cast<uint8>(SectorZoneType::Count), "여기도 확인해주세요");


	const std::unordered_map<std::string, SectorGroupType> SectorGroupTypeConverter
	{
		 { "ForestLevel1", SectorGroupType::ForestLevel1 }
		,{ "ForestLevel2", SectorGroupType::ForestLevel2 }
		,{ "ForestLevel3", SectorGroupType::ForestLevel3 }
		,{ "SeaLevel1", SectorGroupType::SeaLevel1 }
		,{ "SeaLevel2", SectorGroupType::SeaLevel2 }
		,{ "SeaLevel3", SectorGroupType::SeaLevel3 }
		,{ "DesertLevel1", SectorGroupType::DesertLevel1 }
		,{ "DesertLevel2", SectorGroupType::DesertLevel2 }
		,{ "DesertLevel3", SectorGroupType::DesertLevel3 }
		,{ "JungleLevel1", SectorGroupType::JungleLevel1 }
		,{ "JungleLevel2", SectorGroupType::JungleLevel2 }
		,{ "JungleLevel3", SectorGroupType::JungleLevel3 }
	};
	TA_COMPILE_DEV(12 == static_cast<uint8>(SectorGroupType::Count), "여기도 확인해주세요");




#define CONVERT_FUNCTION_DEFINITION(EnumTypeName)											 \
																							 \
	template <>																				 \
	bool ConvertEnumToString(const EnumTypeName& enumValue, std::string& output) noexcept	 \
	{																					     \
		auto it = EnumTypeName##Converter.begin();										     \
		const auto end = EnumTypeName##Converter.end();									     \
		for (; it != end; ++it)															     \
		{																				     \
			if (enumValue == it->second)												     \
			{																			     \
				output = it->first;														     \
				return true;															     \
			}																			     \
		}																				     \
		return false;																	     \
	}																					     \
																						     \
	template <>																			     \
	EnumTypeName ConvertStringToEnum(const std::string& input) noexcept					     \
	{																					     \
		auto target = EnumTypeName##Converter.find(input);								     \
		if (EnumTypeName##Converter.end() == target)									     \
		{																				     \
			TA_ASSERT_DEV(false, "비정상입니다.");										     \
			return EnumTypeName::Count;													     \
		}																				     \
																						     \
		return target->second;															     \
	}																					     \


	CONVERT_FUNCTION_DEFINITION(ActorType)
	CONVERT_FUNCTION_DEFINITION(GameDataType)
	CONVERT_FUNCTION_DEFINITION(AiClassType)
	CONVERT_FUNCTION_DEFINITION(GameWorldType)
	CONVERT_FUNCTION_DEFINITION(ItemType)
	CONVERT_FUNCTION_DEFINITION(TADataType)
	CONVERT_FUNCTION_DEFINITION(InteractionType)
	CONVERT_FUNCTION_DEFINITION(MeshType)
	CONVERT_FUNCTION_DEFINITION(SectorZoneType)
	CONVERT_FUNCTION_DEFINITION(SectorGroupType)




#undef CONVERT_FUNCTION_DEFINITION
}																					 



