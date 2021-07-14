#include "Common/EventGameDataObject.h"
#include "Common/StringUtility.h"
#include "Common/Sector.h"
#include "Common/ContentParameter.h"
#include "Common/ScopedLock.h"
#include "Common/GameData.h"
#include "Common/Vector.h"
#include "Common/CommonActor.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"


namespace ta
{
	EventGameDataObject* EventGameDataObjectFactory::generateConditionGameDataObject(const std::vector<std::string>& strings
																					 , const EventGameDataObjectType& objectType) const noexcept
	{
		EventGameDataObject* rv = nullptr;

		switch (objectType)
		{
		case EventGameDataObjectType::SpawnActor:
			{
				rv = new EventGameDataObjectSpawnActor();
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
	EventGameDataObject::EventGameDataObject(const EventGameDataObjectType& objectType) noexcept
		: _objectType(objectType)
	{
	}

	EventGameDataObject::~EventGameDataObject(void) noexcept
	{
	}
}


namespace ta
{
	EventGameDataObjectSpawnActor::EventGameDataObjectSpawnActor(void) noexcept
		: EventGameDataObject(EventGameDataObjectType::SpawnActor)
	{
	}

	EventGameDataObjectSpawnActor::~EventGameDataObjectSpawnActor(void) noexcept
	{
	}

	bool EventGameDataObjectSpawnActor::execute(const ContentParameter& parameter) const noexcept
	{
		Sector* targetSector = GetSector(parameter._sectorKey);
		if (nullptr == targetSector)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		Vector targetPosition;
		if (false == GetSectorRandomPosition(parameter._sectorKey, targetPosition))
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		CommonActorDetailSpawnDataForCharacter spawnData(targetPosition, Vector(), _targetGroupGameDataKey);
		
		CommonActor* spawnedActor = nullptr;
		spawnedActor = g_app->getActorManager()->createCharacterActorAndInitializeComponentsForServer(spawnData);
		if (nullptr == spawnedActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		{
			ScopedLock sectorLock(targetSector, true);
			if (false == targetSector->registerToOwnedActorsForServer_(_targetGroupGameDataKey, spawnedActor->getActorKey()))
			{
				TA_ASSERT_DEV(false, "비정상입니다");
				return false;
			}
		}

		return true;
	}

	bool EventGameDataObjectSpawnActor::loadFromStrings(const std::vector<std::string>& strings) noexcept
	{
		// 100 => 100번 GroupGameDataKey를 가진 것을 스폰
		const uint32 stringCount = strings.size();
		if (1 != stringCount)
		{
			TA_ASSERT_DEV(false, "비정상입니다");
			return false;
		}

		_targetGroupGameDataKey = FromStringCast<GroupGameDataKeyType>(strings[0]);

		return true;
	}
}
	

