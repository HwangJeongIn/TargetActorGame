#include "Common/AiBlackboard.h"
#include "Common/StringUtility.h"


namespace ta
{
	AiBlackboard::AiBlackboard(void) noexcept
	{
	}

	AiBlackboard::~AiBlackboard(void) noexcept
	{
	}


	const hash_value AiBlackboard::MyActorKeyHash = ComputeHash("MyActorKey");
	const hash_value AiBlackboard::TargetActorKeyHash = ComputeHash("TargetActorKey");
	const hash_value AiBlackboard::PositionHash = ComputeHash("Position");
	const hash_value AiBlackboard::DirectionHash = ComputeHash("Direction");
	const hash_value AiBlackboard::SpeedHash = ComputeHash("Speed");


#define VALUE_FUNCTION_DEFINITION(FunctionValueName, ValueType, GroupName)									  \
																											  \
	bool AiBlackboard::addValueAs##FunctionValueName(const hash_value& name, const ValueType& input) noexcept \
	{																							  			  \
		std::pair<std::unordered_map<hash_value, ValueType>::const_iterator, bool> rv 			  			  \
			= _##GroupName##Group.insert(std::make_pair(name, input));							  			  \
		if (false == rv.second)																	  			  \
		{																						  			  \
			TA_ASSERT_DEV(false, "두번 추가하려 합니다.");													  \
			return false;																		  			  \
		}																						  			  \
		return false;																			  			  \
	}																							  			  \
																								  			  \
	bool AiBlackboard::setValueAs##FunctionValueName(const hash_value& name, const ValueType& input) noexcept \
	{																							  			  \
		std::unordered_map<hash_value, ValueType>::iterator target 								  			  \
			= _##GroupName##Group.find(name);													  			  \
		if (_##GroupName##Group.end() == target)												  			  \
		{																						  			  \
			TA_ASSERT_DEV(false, "해당 값이 없습니다.");														  \
			return false;																		  			  \
		}																						  			  \
		target->second = input;																	  			  \
		return true;																			  			  \
	}																							  			  \
																								  			  \
	bool AiBlackboard::getValueAs##FunctionValueName(const hash_value& name, ValueType& output) noexcept	  \
	{																							   			  \
		std::unordered_map<hash_value, ValueType>::const_iterator target 						   			  \
			= _##GroupName##Group.find(name);													   			  \
		if (_##GroupName##Group.end() == target)												   			  \
		{																						   			  \
			TA_ASSERT_DEV(false, "해당 값이 없습니다.");														  \
			return false;																		   			  \
		}																						   			  \
		output = target->second;																   			  \
		return true;																			   			  \
	}

	VALUE_FUNCTION_DEFINITION(Float, float, float)
	VALUE_FUNCTION_DEFINITION(Bool, bool, bool)
	VALUE_FUNCTION_DEFINITION(Vector, Vector, vector)
	VALUE_FUNCTION_DEFINITION(Uint32, uint32, uint32)
	VALUE_FUNCTION_DEFINITION(Int32, int32, int32)


#undef VALUE_FUNCTION_DEFINITION

}
