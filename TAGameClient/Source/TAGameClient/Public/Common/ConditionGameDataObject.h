#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"

namespace ta
{
	TA_COMPILE_DEV(1 == static_cast<uint8>(ConditionGameDataObjectType::Count), "여기도 추가해주세요");
	class ConditionGameDataObject;
	class ContentParameter;
}


namespace ta
{
	class ConditionGameDataObjectFactory
	{
	public:
		ConditionGameDataObject* generateConditionGameDataObject(const std::vector<std::string>& strings
																 , const ConditionGameDataObjectType& objectType
																 , bool isNot) const noexcept;
	};
}


namespace ta
{
	class ConditionGameDataObject
	{
	public:
		explicit ConditionGameDataObject(const ConditionGameDataObjectType& objectType, bool isNot) noexcept;
		virtual ~ConditionGameDataObject(void) noexcept;
		
		virtual bool checkConditionDetail(const ContentParameter& parameter) const noexcept = 0;
		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept = 0;
	
		bool checkCondition(const ContentParameter& parameter) const noexcept;
	public:
		ConditionGameDataObjectType _objectType;
		bool _isNot;
	};
}


namespace ta
{
	class ConditionGameDataObjectLimitCount : public ConditionGameDataObject
	{
	public:
		explicit ConditionGameDataObjectLimitCount(bool isNot) noexcept;
		virtual ~ConditionGameDataObjectLimitCount(void) noexcept;
		
		virtual bool checkConditionDetail(const ContentParameter& parameter) const noexcept override final;
		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept override final;
	
	public:
		GroupGameDataKey _targetGroupGameDataKey;
		uint32 _maxCount;
	};
}