#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"

namespace ta
{
	TA_COMPILE_DEV(1 == static_cast<uint8>(EventGameDataObjectType::Count), "여기도 추가해주세요");
	class EventGameDataObject;
	class ContentParameter;
	class ConditionGameData;
}


namespace ta
{
	class EventGameDataObjectFactory
	{
	public:
		EventGameDataObject* generateConditionGameDataObject(const std::vector<std::string>& strings
																 , const EventGameDataObjectType& objectType) const noexcept;
	};
}


namespace ta
{
	class EventGameDataObject
	{
	public:
		explicit EventGameDataObject(const EventGameDataObjectType& objectType) noexcept;
		virtual ~EventGameDataObject(void) noexcept;

		//void setConditioinGameDataList(const std::vector<const ConditionGameData*> input) noexcept;

		virtual bool execute(const ContentParameter& parameter) const noexcept = 0;
		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept = 0;

	public:
		EventGameDataObjectType _objectType;

		// 사용하는 측에서 컨디션 체크하고 들어오는게 맞는 것 같다.
		// 순수하게 기능만 있는 이벤트 오브젝트로 설계
		//std::vector<const ConditionGameData*> _conditions;
	};
}


namespace ta
{
	class EventGameDataObjectSpawnActor : public EventGameDataObject
	{
	public:
		explicit EventGameDataObjectSpawnActor(void) noexcept;
		virtual ~EventGameDataObjectSpawnActor(void) noexcept;

		virtual bool execute(const ContentParameter& parameter) const noexcept override final;
		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept override final;

	public:
		GroupGameDataKey _targetGroupGameDataKey;
	};
}