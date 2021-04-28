#pragma once
#include "Common/CommonDataTypeDefinition.h"
#include "Common/CommonLog.h"
#include <string>
#include <unordered_map>
#include <functional>



namespace ta
{
	template <typename T>
	bool ConvertEnumToString(const T& enumValue, std::string& output) noexcept
	{
		TA_COMPILE_DEV(false, "비정상입니다.");
		//return false;
	}

	template <typename T>
	T ConvertStringToEnum(const std::string& input) noexcept
	{
		TA_COMPILE_DEV(false, "비정상입니다.");
		//return T::Count;
	}


#define CONVERT_FUNCTION_DECLARATION(EnumTypeName)											  \
	template <>																				  \
	bool ConvertEnumToString(const EnumTypeName& enumValue, std::string& output) noexcept;	  \
																							  \
	template <>																				  \
	EnumTypeName ConvertStringToEnum(const std::string& input) noexcept;					  \


	CONVERT_FUNCTION_DECLARATION(ActorType)
	CONVERT_FUNCTION_DECLARATION(GameDataType)
	CONVERT_FUNCTION_DECLARATION(AiClassType)
	CONVERT_FUNCTION_DECLARATION(GameWorldType)
	CONVERT_FUNCTION_DECLARATION(ItemType)
	CONVERT_FUNCTION_DECLARATION(TADataType)
	CONVERT_FUNCTION_DECLARATION(InteractionType)
	CONVERT_FUNCTION_DECLARATION(MeshType)
	CONVERT_FUNCTION_DECLARATION(SectorZoneType)
	CONVERT_FUNCTION_DECLARATION(SectorGroupType)



#undef CONVERT_FUNCTION_DECLARATION
}
