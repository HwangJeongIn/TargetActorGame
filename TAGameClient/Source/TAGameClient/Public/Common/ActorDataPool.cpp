#include "Common/ActorDataPool.h"
#include "Common/ScopedLock.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/MessagePack.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonActionActorComponent.h"
#include "Common/CommonAiActorComponent.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/CommonInventoryActorComponent.h"
#include "Common/ActorComponentTypeList.h"
#include "Common/ActorComponentPool.h"


namespace ta
{
	ActorDataPool::ActorDataPool(void) noexcept
	{
	}

	ActorDataPool::~ActorDataPool(void) noexcept
	{
	}

	bool ActorDataPool::initialize(void) noexcept
	{
		_actorPoolValues = nullptr;
		
		_moveComponentPool = new ActorComponentPool;
		_actionComponentPool = new ActorComponentPool;
		_aiComponentPool = new ActorComponentPool;
		_characterComponentPool = new ActorComponentPool;
		_inventoryComponentPool = new ActorComponentPool;

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		if (false == initializeAllComponentCountFromActorType())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	bool ActorDataPool::open(void) noexcept
	{
		MaxPlayerActorDataPoolCapacity;
		MaxNpcActorDataPoolCapacity;
		MaxObjectActorDataPoolCapacity;

		ActorType OwnerActorType = ActorType::Player;
		uint32 ActorTypeCount = MaxPlayerActorDataPoolCapacity;

#define OPEN_ACTORTYPE(OwnerActorType, ActorTypeCount)																		


		_freeIndex.reserve(_maxCount -1);


		// 0번인덱스 제외
		CommonActor* actor = nullptr;
		for (uint32 index = ActorTypeCount - 1; index >= 0; --index)
		{
			actor = getActor(index);
			actor->setActorKey_(index);

			uint8 componentCount = static_cast<uint8>(ActorComponentType::Count);
			for (uint8 componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				getActorComponent(index, static_cast<ActorComponentType>(componentIndex))->setOwner_(actor);
			}
			
			_freeIndex.push_back(index);
		}

		_freeIndex.pop_back();

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		// 0번 인덱스 바로 OwnerActor로 사용 //

		// 액터매니저 객체자체는 초기화되어있는 상태 함수만 사용하니까 상관없을듯
		CommonActor* ownerActor = g_app->getActorManager()->createOwnerActor();
		g_app->getActorManager()->initializeOwnerComponents(ownerActor->getActorKey());

		return true;
	}

	void ActorDataPool::close(void) noexcept
	{
		_playerFreeIndexes.clear();
		_npcFreeIndexes.clear();
		_objectFreeIndexes.clear();

		// 클라 서버마다 있는 컴포넌트와 없는 컴포넌트가 있을 수 있음

		if (nullptr != _actorPoolValues)
		{
			delete[] _actorPoolValues;
		}
		
		if (nullptr != _moveComponentPool)
		{
			delete _moveComponentPool;
		}
		
		if (nullptr != _actionComponentPool)
		{
			delete _actionComponentPool;
		}

		if (nullptr != _aiComponentPool)
		{
			delete _aiComponentPool;
		}

		if (nullptr != _characterComponentPool)
		{
			delete _characterComponentPool;
		}

		if (nullptr != _inventoryComponentPool)
		{
			delete _inventoryComponentPool;
		}

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
	}

	CommonActor* ActorDataPool::fromPool(const ActorKey& targetActorKey, CommonActorBasicSpawnData& spawnData) noexcept
	{
		return fromPool(spawnData, true, targetActorKey);
	}

	CommonActor* ActorDataPool::fromPool(CommonActorBasicSpawnData& spawnData
										 , const bool forceTo /*= false*/
										 , const ActorKey& actorKey/* = ActorKey()*/) noexcept
	{
		// freelist삭제 후 초기화

		uint32 targetIndex = 0;
		// 비어있는 인덱스 검증
		if(false == forceTo)
		{
			// 잠깐 걸고 인덱스만 빼오는거라 괜찮을듯
			ScopedLock lock(this);

			if (true == _freeIndex.empty())
			{
				TA_ASSERT_DEV(false, "풀이 가득찼습니다 대책을 마련해야합니다.");
				return nullptr;
			}
			else
			{
				targetIndex = _freeIndex.back();
				_freeIndex.pop_back();
			}
		}
		else
		{
			targetIndex = actorKey.getKeyValue();
		}

		CommonActor* targetActor = nullptr;
		{
			targetActor = getActor(targetIndex);
			ScopedLock lock(targetActor, true);

			if (true == targetActor->getInUse_())
			{
				TA_ASSERT_DEV(false, "free list 코드버그입니다 확인해야합니다.");
				return nullptr;
			}
		}


		// 풀에서 해당 컴포넌트 타입에 맞는 포인터로 채워준다.
		{
			const uint32 count = spawnData._componentTypeList.size();
			for (uint32 index = 0; index < count; ++index)
			{
				const ActorComponentType componentType = static_cast<ActorComponentType>(spawnData._componentTypeList[index]);
				ActorComponent* targetComponent = getActorComponent(targetIndex, componentType);

				ScopedLock lock(targetComponent, true);
				if (true == targetComponent->getInUse_())
				{														
					TA_ASSERT_DEV(false, "free list 코드버그입니다 확인해야합니다."); 
					break;												
				}		

				spawnData._componentList.push_back(targetComponent);
			}
		}

		targetActor->onFromPool(spawnData);

		return targetActor;
	}

	bool ActorDataPool::initializeActorComponent(const ActorKey& actorKey, const ComponentData* data, const bool needToBeDeleted) noexcept
	{
		CommonActor* targetActor = getActor(actorKey);
		ActorComponent* actorComponent = nullptr;
		bool rv = true;
		{
			ScopedLock lock(targetActor);
			
			// 현재 초기화 중이여야한다.
			if (false == targetActor->checkState_(PoolValueState::Initializing))
			{
				TA_ASSERT_DEV(false, "초기화 단계가 아닌 액터의 컴포넌트를 초기화하려합니다.");
				rv = false;
			}
			else
			{
				actorComponent = getActorComponent(actorKey, data->getActorComponentType());
				//ScopedLock componentLock(actorComponents);
				// 락 안걸고 처리 락이 필요한 경우에만 안에서 걸게 바꿨다.
				rv = actorComponent->initializeData(data);
			}
		}

		if (true == rv)
		{
			{
				ScopedLock componentLock(actorComponent);
				actorComponent->setComponentValid_(true);
			}

			if (false == targetActor->decreaseCountOfComponentsToInitialize())
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
			}
		}

		if (true == needToBeDeleted)
		{
			delete data;
		}
		
		return rv;
	}

	bool ActorDataPool::toPool(const ActorKey& actorKey, const bool forceTo/* = false*/) noexcept
	{
		// 반납마치고 freelist추가
		if (false == actorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return false;
		}

		CommonActor* targetActor = getActor(actorKey);
		targetActor->onToPool();
		const uint32 index = actorKey.getKeyValue();

		if(false == forceTo)
		{
			ScopedLock lock(this);
			_freeIndex.push_back(index);
		}

		return true;
	}

	const bool ActorDataPool::isFull(void) noexcept
	{
		{
			ScopedLock lock(this);
			return (false == _freeIndex.empty());
		}
	}

	uint32 ActorDataPool::getMaxCount(void) const noexcept
	{
		return _maxCount;
	}

	void ActorDataPool::logTest(void) noexcept
	{
		uint32 freeIndexCount = 0;
		{
			ScopedLock lock(this, true);
			freeIndexCount = _freeIndex.size();
		}

		TA_LOG_DEV("FreeIndex Count : %d\n\n", freeIndexCount);

		bool rv = false;
		for (uint32 index = 0; index < _maxCount; ++index)
		{
			CommonActor* actor = getActor(index);
			{
				ScopedLock lock(actor, true);
				rv = actor->isActive_();
			}

			TA_LOG_DEV("ActorKey : %d is %d , ", index, rv);
		}
	}

	ActorType ActorDataPool::getActorType(const ActorKey& actorKey) noexcept
	{
		uint32 actorKeyValue = actorKey.getKeyValue();
		if (MaxActorDataPoolCapacity <= actorKeyValue)
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return ActorType::Count;
		}

		if (MaxPlayerActorDataPoolCapacity <= actorKeyValue)
		{
			actorKeyValue -= MaxPlayerActorDataPoolCapacity;
		}

		if (MaxNpcActorDataPoolCapacity <= actorKeyValue)
		{
			actorKeyValue -= MaxNpcActorDataPoolCapacity;
		}

		// 들어올일 없다.
		//if (MaxObjectActorDataPoolCapacity <= result)
		//{
		//	result -= MaxObjectActorDataPoolCapacity;
		//}

		return ActorType::Count;
	}

	const bool ActorDataPool::getRelativeGroupIndex(const ActorKey& actorKey, uint32& relativeGroupIndex) const noexcept
	{
		// 플레이어 => Npc => Object 순서대로 액터풀에 들어가있다.
		// 각 그룹에서의 인덱스를 구해준다.
		uint32 actorKeyValue = actorKey.getKeyValue();
		if (MaxActorDataPoolCapacity <= actorKeyValue)
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return false;
		}

		if (MaxPlayerActorDataPoolCapacity <= actorKeyValue)
		{
			actorKeyValue -= MaxPlayerActorDataPoolCapacity;
		}

		if (MaxNpcActorDataPoolCapacity <= actorKeyValue)
		{
			actorKeyValue -= MaxNpcActorDataPoolCapacity;
		}

		// 들어올일 없다.
		//if (MaxObjectActorDataPoolCapacity <= result)
		//{
		//	result -= MaxObjectActorDataPoolCapacity;
		//}

		relativeGroupIndex = actorKeyValue;

		return true;
	}

	bool ActorDataPool::initializeAllComponentCountFromActorType(void) noexcept
	{
#define INITIALIZE_COMPONENT_COUNT(OwnerActorType)																								\
																																				\
		std::unordered_map<ActorType, ActorComponentGroupData>::const_iterator it = ActorComponentGroups.find(OwnerActorType);					\
		if (ActorComponentGroups.end() == it)																									\
		{																																		\
			TA_ASSERT_DEV(false, "비정상");																										\
			return false;																														\
		}																																		\
		const std::vector<ActorComponentType>& componentTypeList = it->second._componentTypeList;												\
		const uint32 componentTypeCount = componentTypeList.size();																				\
		for (uint32 index = 0; index < componentTypeCount; ++index)																				\
		{																																		\
			if (false == addComponentCountFromActorType(OwnerActorType, componentTypeList[index], it->second._countPerComponent))				\
			{																																	\
				TA_ASSERT_DEV(false, "비정상");																									\
				return false;																													\
			}																																	\
		}																																		\


		INITIALIZE_COMPONENT_COUNT(ActorType::Player)
		INITIALIZE_COMPONENT_COUNT(ActorType::Npc)
		INITIALIZE_COMPONENT_COUNT(ActorType::Object)

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");

#undef INITIALIZE_COMPONENT_COUNT

		return true;

	}

	bool ActorDataPool::addComponentCountFromActorType(const ActorType& ownerActorType, const ActorComponentType& actorComponentType, const uint32& count) noexcept
	{
		switch (actorComponentType)
		{
#define ADD_COMPONENT_COUNT(ComponentType, PoolName)													\
			case ActorComponentType::ComponentType:														\
				{																						\
					if(false == PoolName->addComponentCountFromActorType(ownerActorType, count))		\
					{																					\
						TA_ASSERT_DEV(false, "비정상");													\
						return false;																	\
					}																					\
				}																						\
				break;																					\


				ADD_COMPONENT_COUNT(Move, _moveComponentPool)
				ADD_COMPONENT_COUNT(Action, _actionComponentPool)
				ADD_COMPONENT_COUNT(Ai, _aiComponentPool)
				ADD_COMPONENT_COUNT(Character, _characterComponentPool)
				ADD_COMPONENT_COUNT(Inventory, _inventoryComponentPool)

				TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

#undef INITIALIZE_COMPONENT_COUNT

		default:
			break;

		}

		return true;
	}
}

