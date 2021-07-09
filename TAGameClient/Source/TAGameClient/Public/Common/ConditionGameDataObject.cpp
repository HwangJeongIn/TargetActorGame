#include "Common/ConditionGameDataObject.h"

namespace ta
{
	ConditionGameDataObjectParameter::ConditionGameDataObjectParameter(const ActorKey& myActorKey
																	   , const ActorKey& targetActorKey
																	   , const SectorKey& sectorKey) noexcept
		: _myActorKey(myActorKey)
		, _targetActorKey(targetActorKey)
		, _sectorKey(sectorKey)
	{

	}
}


namespace ta
{
	ConditionGameDataObject* ConditionGameDataObjectFactory::generateConditionGameDataObject(const std::vector<std::string>& strings
																							 , const ConditionGameDataObjectType& objectType
																							 , bool isNot) const noexcept
	{
		ConditionGameDataObject* rv = nullptr;

		switch (objectType)
		{
		case ConditionGameDataObjectType::LimitCount:
			{
				rv = new ConditionGameDataObjectLimitCount(isNot);
			}
			break;
		default:
			{
				TA_COMPILE_DEV(1 == static_cast<uint8>(ConditionGameDataObjectType::Count), "여기도 추가해주세요");
				TA_ASSERT_DEV(false, "비정상");
				return nullptr;
			}
			break;
		}

		if (false == rv->loadFromStrings(strings))
		{
			delete rv;
			TA_ASSERT_DEV(false, "비정상");
			return nullptr;
		}

		return rv;
	}
}


namespace ta
{
	ConditionGameDataObject::ConditionGameDataObject(const ConditionGameDataObjectType& objectType, bool isNot) noexcept
		: _objectType(objectType)
		, _isNot(isNot)
	{
	}

	ConditionGameDataObject::~ConditionGameDataObject(void) noexcept
	{
	}

	bool ConditionGameDataObject::checkCondition(ConditionGameDataObjectParameter& parameter) const noexcept
	{
		const bool rv = checkConditionDetail(parameter);

		if (true == _isNot)
		{
			return !rv;
		}
		else
		{
			return rv;
		}
	}
}


namespace ta
{
	ConditionGameDataObjectLimitCount::ConditionGameDataObjectLimitCount(bool isNot) noexcept
		: ConditionGameDataObject(ConditionGameDataObjectType::LimitCount, isNot)
	{
	}

	ConditionGameDataObjectLimitCount::~ConditionGameDataObjectLimitCount(void) noexcept
	{
	}

	bool ConditionGameDataObjectLimitCount::checkConditionDetail(ConditionGameDataObjectParameter& parameter) const noexcept
	{
		// 구현해야 합니다.
		return true;
	}
	
	bool ConditionGameDataObjectLimitCount::loadFromStrings(const std::vector<std::string>& strings) noexcept
	{
		return false;
	}
}
