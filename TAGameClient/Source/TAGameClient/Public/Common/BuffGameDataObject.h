#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"

namespace ta
{
	TA_COMPILE_DEV(3 == static_cast<uint8>(BuffGameDataObjectType::Count), "여기도 추가해주세요");
	class BuffGameDataObject;
	class ContentParameter;
	class ConditionGameData;
}


namespace ta
{
	class BuffGameDataObjectFactory
	{
	public:
		BuffGameDataObject* generateBuffGameDataObject(const std::vector<std::string>& strings
															 , const BuffGameDataObjectType& objectType) const noexcept;
	};
}


namespace ta
{
	class BuffGameDataObject
	{
	public:
		explicit BuffGameDataObject(const BuffGameDataObjectType& objectType) noexcept;
		virtual ~BuffGameDataObject(void) noexcept;

		virtual bool doBuff(const ContentParameter& parameter) const noexcept = 0;
		virtual bool undoBuff(const ContentParameter& parameter) const noexcept;
		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept = 0;

		bool isOneTime(void) const noexcept;
		uint32 getInterval(void) const noexcept;


	public:
		BuffGameDataObjectType _objectType;

		
		uint32 _duration; // 0초면 1회성
		uint32 _interval; // duration동안 interval 간격으로 효과가 있을 경우
	};
}


namespace ta
{
	class BuffGameDataObjectStat : public BuffGameDataObject
	{
	public:
		explicit BuffGameDataObjectStat(void) noexcept;
		virtual ~BuffGameDataObjectStat(void) noexcept;

		virtual bool doBuff(const ContentParameter& parameter) const noexcept override final;
		virtual bool undoBuff(const ContentParameter& parameter) const noexcept override final;

		virtual bool loadFromStrings(const std::vector<std::string>& strings) noexcept override final;

	public:
		CharacterStatType _statType;
		float _point;
	};
}