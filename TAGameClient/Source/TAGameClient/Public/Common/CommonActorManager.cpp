#include "Common/CommonActorManager.h"
#include "Common/KeyDefinition.h"
#include "Common/ActorDataGroups.h"
#include "Common/ActorDataPool.h"
#include "Common/Sector.h"
#include "Common/ComponentData.h"
#include "Common/ScopedLock.h"


namespace ta
{
	CommonActorManager::CommonActorManager(void) noexcept
	{
	}

	CommonActorManager::~CommonActorManager(void) noexcept
	{
	}

	bool CommonActorManager::initialize(void) noexcept
	{
		if (false == _actorDataPool->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	bool CommonActorManager::open(void) noexcept
	{
		if (false == _actorDataPool->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		for (uint32 index = 0; index < CountOfSectors; ++index)
		{
			_allSectors[index].setSectorKey(SectorKey(index));
		}

		return true;
	}

	void CommonActorManager::close(void) noexcept
	{
		_actorDataPool->close();

		delete _actorDataPool;
		delete[] _allSectors;
	}

	CommonActor* CommonActorManager::getOwnerActor(void) noexcept
	{
		return _actorDataPool->getActor(ActorKey(0));
	}

	const ActorKey CommonActorManager::getOwnerActorKey(void) const noexcept
	{
		return ActorKey(0);
	}

	CommonActor* CommonActorManager::getActor(const ActorKey& actorKey, bool activeActorOnly/* = false*/) noexcept
	{
		CommonActor* targetActor = _actorDataPool->getActor(actorKey);
		if (true == activeActorOnly)
		{
			ScopedLock actorLock(targetActor, true);
			if (false == targetActor->isActive_())
			{
				return nullptr;
			}
		}

		return targetActor;
	}

	ActorComponent* CommonActorManager::getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept
	{
		return _actorDataPool->getActorComponent(actorKey, componentType);
	}

	bool CommonActorManager::checkSectorValid(const SectorKey& sectorKey) const noexcept
	{
		if (false == sectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const int32 index = sectorKey.getKeyValue();
		if (CountOfSectors <= index)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	CommonActor* CommonActorManager::createOwnerActor(void) noexcept
	{
		CommonActorBasicSpawnData spawnData;
		spawnData.initializeComponentsAsOwner();
		spawnData._networkOwnerType = NetworkOwnerType::Mine;
		return _actorDataPool->fromPool(spawnData, true, ActorKey(0));
	}

	bool CommonActorManager::initializeFixedComponents(const ActorKey& actorKey, const std::vector<ActorComponentType>& typeList) noexcept
	{
		uint32 count = typeList.size();
		for (uint32 index = 0; index < count; ++index)
		{
			const ActorComponentType componentType = static_cast<ActorComponentType>(typeList[index]);

			ComponentData* componentData = createFixedComponentData(componentType);
			if (nullptr == componentData)
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
				return false;
			}

			if (false == initializeActorComponent(actorKey, componentData, true))
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
				return false;
			}
		}

		return true;
	}

	bool CommonActorManager::initializeDefaultActorComponents(const ActorKey& actorKey) noexcept
	{
		return initializeFixedComponents(actorKey, DefaultActor);
	}

	bool CommonActorManager::initializeOwnerComponents(const ActorKey& actorKey) noexcept
	{
		return initializeFixedComponents(actorKey, OwnerActor);
	}
	
	bool CommonActorManager::initializePlayerComponents(const ActorKey& actorKey) noexcept
	{
		return initializeFixedComponents(actorKey, PlayerActor);
	}

	bool CommonActorManager::initializeMonsterComponents(const ActorKey& actorKey) noexcept
	{
		return initializeFixedComponents(actorKey, MonsterActor);
	}

	ComponentData* CommonActorManager::createFixedComponentData(const ActorComponentType& actorComponent) noexcept
	{
		ComponentData* rv = nullptr;
		switch (actorComponent)
		{
#define CREATE_COMPONENT_DATA(TypeName, ComponentName) \
        case ActorComponentType::TypeName:             \
            {                                          \
                rv = new ComponentName();              \
            }                                          \
            break;

			CREATE_COMPONENT_DATA(Move, CommonMoveComponentData)
			CREATE_COMPONENT_DATA(Action, CommonActionComponentData)
			CREATE_COMPONENT_DATA(Ai, CommonAiComponentData)
			CREATE_COMPONENT_DATA(Character, CommonCharacterComponentData)
			CREATE_COMPONENT_DATA(Inventory, CommonInventoryComponentData)

			TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");


#undef CREATE_COMPONENT_DATA

		default:
			{
				TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			}
			break;
		}
		return rv;
	}

	bool CommonActorManager::initializeActorComponent(const ActorKey& actorKey, const ComponentData* data, const bool needToBeDeleted /*= true*/) noexcept
	{
		return _actorDataPool->initializeActorComponent(actorKey, data, needToBeDeleted);
	}

	bool CommonActorManager::destroyActor(const ActorKey& actorKey) noexcept
	{
		if (false == actorKey.isValid())
		{
			TA_ASSERT_DEV(false, "삭제에 실패했습니다.")
			return false;
		}

		return _actorDataPool->toPool(actorKey);
	}

	bool CommonActorManager::destroyActor(const CommonActor* actor) noexcept
	{
		if (nullptr == actor)
		{
			return false;
		}

		if (false == destroyActor(actor->getActorKey()))
		{
			TA_ASSERT_DEV(false, "삭제에 실패했습니다.")
			return false;
		}

		return true;
	}
	
	void CommonActorManager::logTest(void) noexcept
	{
		_actorDataPool->logTest();
	}
}

