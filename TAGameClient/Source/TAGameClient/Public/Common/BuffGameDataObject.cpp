#include "Common/BuffGameDataObject.h"
#include "Common/StringUtility.h"
#include "Common/Sector.h"
#include "Common/ContentParameter.h"
#include "Common/ScopedLock.h"
#include "Common/GameData.h"
#include "Common/Vector.h"
#include "Common/CommonActor.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/EnumUtility.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/CommonCharacterActorSystem.h"


namespace ta
{
	BuffGameDataObject* BuffGameDataObjectFactory::generateBuffGameDataObject(const std::vector<std::string>& strings
																			  , const BuffGameDataObjectType& objectType) const noexcept
	{
		BuffGameDataObject* rv = nullptr;

		switch (objectType)
		{
		case BuffGameDataObjectType::Stat:
			{
				rv = new BuffGameDataObjectStat();
			}
			break;

		default:
			{
				TA_COMPILE_DEV(3 == static_cast<uint8>(BuffGameDataObjectType::Count), "여기도 추가해주세요");
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
	BuffGameDataObject::BuffGameDataObject(const BuffGameDataObjectType& objectType) noexcept
		: _objectType(objectType)
	{
	}

	BuffGameDataObject::~BuffGameDataObject(void) noexcept
	{
	}

	bool BuffGameDataObject::undoBuff(const ContentParameter& parameter) const noexcept
	{
		return true;
	}

	bool BuffGameDataObject::isOneTime(void) const noexcept
	{
		return 0 == _duration ? true : false;
	}
	
	uint32 BuffGameDataObject::getInterval(void) const noexcept
	{
		if (0 == _interval)
		{
			return _duration;
		}

		return _interval;
	}
}


namespace ta
{
	BuffGameDataObjectStat::BuffGameDataObjectStat(void) noexcept
		: BuffGameDataObject(BuffGameDataObjectType::Stat)
		, _point(0)
		, _statType(CharacterStatType::Count)
	{
	}

	BuffGameDataObjectStat::~BuffGameDataObjectStat(void) noexcept
	{
	}

	bool BuffGameDataObjectStat::doBuff(const ContentParameter& parameter) const noexcept
	{
		CommonActor* myActor = g_app->getActorManager()->getActor(parameter._myActorKey);

		if (nullptr == myActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		CommonCharacterActorComponent* myCharacterComponent = GetActorComponent<CommonCharacterActorComponent>(parameter._myActorKey);

		if (nullptr == myCharacterComponent)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		CommonCharacterActorSystem* characterActorSystem = GetActorSystem<CommonCharacterActorSystem>();

		if (false == characterActorSystem->addStat(myCharacterComponent, _statType, _point))
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		return true;
	}

	bool BuffGameDataObjectStat::undoBuff(const ContentParameter& parameter) const noexcept
	{
		CommonActor* myActor = g_app->getActorManager()->getActor(parameter._myActorKey);

		if (nullptr == myActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		CommonCharacterActorComponent* myCharacterComponent = GetActorComponent<CommonCharacterActorComponent>(parameter._myActorKey);

		if (nullptr == myCharacterComponent)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		CommonCharacterActorSystem* characterActorSystem = GetActorSystem<CommonCharacterActorSystem>();

		// 올라간 Mp, Hp를 다시 내리지 않는다.
		if (CharacterStatType::Hp == _statType || CharacterStatType::Mp == _statType)
		{
			return true;
		}

		if (false == characterActorSystem->addStat(myCharacterComponent, _statType, -_point))
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		return true;
	}

	bool BuffGameDataObjectStat::loadFromStrings(const std::vector<std::string>& strings) noexcept
	{
		const uint32 stringCount = strings.size();
		// Hp(500)
		if (4 != stringCount)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		_statType = ConvertStringToEnum<CharacterStatType>(strings[0]);
		_point = FromStringCast<float>(strings[2]);

		return true;
	}
}


