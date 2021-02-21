#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include <string>
#include <unordered_map>


namespace ta
{
	class AiBlackboard
	{
	public:
		AiBlackboard(void) noexcept;
		~AiBlackboard(void) noexcept;

#define VALUE_FUNCTION_DECLARATION(FunctionValueName, ValueType)									   \
		bool addValueAs##FunctionValueName(const hash_value& name, const ValueType& input) noexcept;   \
		bool setValueAs##FunctionValueName(const hash_value& name, const ValueType& input) noexcept;   \
		bool getValueAs##FunctionValueName(const hash_value& name, ValueType& output) noexcept;		   \

		VALUE_FUNCTION_DECLARATION(Float, float)
		VALUE_FUNCTION_DECLARATION(Bool, bool)
		VALUE_FUNCTION_DECLARATION(Vector, Vector)
		VALUE_FUNCTION_DECLARATION(Uint32, uint32)
		VALUE_FUNCTION_DECLARATION(Int32, int32)

#undef VALUE_FUNCTION_DECLARATION

	public:
		static const hash_value MyActorKeyHash;
		static const hash_value TargetActorKeyHash;
		static const hash_value PositionHash;
		static const hash_value DirectionHash;
		static const hash_value SpeedHash;

	private:
		std::unordered_map<hash_value, float>	_floatGroup;
		std::unordered_map<hash_value, bool>	_boolGroup;
		std::unordered_map<hash_value, Vector>	_vectorGroup;
		std::unordered_map<hash_value, uint32>	_uint32Group;
		std::unordered_map<hash_value, int32>	_int32Group;
	};
}