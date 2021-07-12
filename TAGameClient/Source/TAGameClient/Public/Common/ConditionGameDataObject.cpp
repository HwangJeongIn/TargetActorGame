#include "Common/ConditionGameDataObject.h"
#include "Common/StringUtility.h"
#include "Common/Sector.h"
#include "Common/ScopedLock.h"
#include "Common/ContentParameter.h"



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

	bool ConditionGameDataObject::checkCondition(ContentParameter& parameter) const noexcept
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

	bool ConditionGameDataObjectLimitCount::checkConditionDetail(ContentParameter& parameter) const noexcept
	{
		Sector* targetSector = GetSector(parameter._sectorKey);
		if (nullptr == targetSector)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		uint32 targetGroupGameDataKeyCount = 0;
		{
			ScopedLock sectorLock(targetSector, true);
			targetGroupGameDataKeyCount = targetSector->getOwnedActorCountForServer_(_targetGroupGameDataKey);
		}
		
		return targetGroupGameDataKeyCount < _maxCount ? true : false;
	}
	
	bool ConditionGameDataObjectLimitCount::loadFromStrings(const std::vector<std::string>& strings) noexcept
	{
		// 100,3 => 100번 GroupGameDataKey를 가진 것을 3으로 제한 // 괄호는 빠져서 들어옴
		const uint32 stringCount = strings.size();
		if (3 != stringCount)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}
		
		_targetGroupGameDataKey = FromStringCast<GroupGameDataKeyType>(strings[0]);
		_maxCount = FromStringCast<uint32>(strings[2]);

		return true;
	}
}
