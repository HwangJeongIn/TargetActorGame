#pragma once
#include "Common/CommonDataTypeDefinition.h"
#include "Common/CommonLog.h"
#include <string>
#include <unordered_map>
#include <functional>



namespace ta
{
	template <typename T>
	bool ConvertEnumToString(T& enumValue, std::string& output) noexcept
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
	bool ConvertEnumToString(EnumTypeName& enumValue, std::string& output) noexcept;		  \
																							  \
	template <>																				  \
	EnumTypeName ConvertStringToEnum(const std::string& input) noexcept;					  \


	CONVERT_FUNCTION_DECLARATION(ActorType)
	CONVERT_FUNCTION_DECLARATION(GameDataType)
	CONVERT_FUNCTION_DECLARATION(AiClassType)
	CONVERT_FUNCTION_DECLARATION(GameWorldType)
	CONVERT_FUNCTION_DECLARATION(ItemType)

#undef CONVERT_FUNCTION_DECLARATION
}
