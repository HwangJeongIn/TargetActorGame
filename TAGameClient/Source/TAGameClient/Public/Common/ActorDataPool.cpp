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
#include "Common/ActorDataGroups.h"
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
		
		_moveComponentPool = new ActorComponentPool(ActorComponentType::Move);
		_actionComponentPool = new ActorComponentPool(ActorComponentType::Action);
		_aiComponentPool = new ActorComponentPool(ActorComponentType::Ai);
		_characterComponentPool = new ActorComponentPool(ActorComponentType::Character);
		_inventoryComponentPool = new ActorComponentPool(ActorComponentType::Inventory);
		_objectComponentPool = new ActorComponentPool(ActorComponentType::Object);

		TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

		if (false == initializeAllComponentCountFromActorType())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	bool ActorDataPool::open(void) noexcept
	{
		std::unordered_map<ActorType, ActorGroup>::const_iterator it = ActorDataGroups.begin();
		const std::unordered_map<ActorType, ActorGroup>::const_iterator end = ActorDataGroups.end();
		int32 currentBoundary = 0;
		for (; end != it; ++it)
		{
			const int32 currentActorTypeActorCount = static_cast<int32>(it->second._count);
			for (int32 actorIndex = (currentBoundary + currentActorTypeActorCount - 1); actorIndex >= currentBoundary; --actorIndex)
			{
				CommonActor* actor = getActor(actorIndex);

				// 이 두개의 값은 불변
				actor->setActorKey_(actorIndex);
				actor->setActorType_(it->first);

				uint8 componentCount = it->second._componentTypeList.size();
				for (uint8 componentTypeIndex = 0; componentTypeIndex < componentCount; ++componentTypeIndex)
				{
					getActorComponent(actorIndex, it->second._componentTypeList[componentTypeIndex])->setOwner_(actor);
				}

				LockableObject* targetFreeIndexesLock = nullptr;
				std::vector<uint32>* targetFreeIndexes = nullptr;
				if (false == getFreeIndexesAndLock(it->first, targetFreeIndexesLock, targetFreeIndexes))
				{
					TA_ASSERT_DEV(false, "비정상");
					return false;
				}

				targetFreeIndexes->push_back(actorIndex);
			}

			currentBoundary += currentActorTypeActorCount;
		}
		_playerFreeIndexes.pop_back();

		// 0번 인덱스 바로 OwnerActor로 사용
		// 액터매니저 객체자체는 초기화되어있는 상태 함수만 사용하니까 상관없을듯
		CommonActor* ownerActor = g_app->getActorManager()->createOwnerActor();
		g_app->getActorManager()->initializeOwnerFixedComponents(ownerActor->getActorKey());

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

		if (nullptr != _objectComponentPool)
		{
			delete _objectComponentPool;
		}

		TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
	}

	CommonActor* ActorDataPool::fromPool(const ActorKey& targetActorKey, CommonActorBasicSpawnData& spawnData) noexcept
	{
		return fromPool(spawnData, true, targetActorKey);
	}

	CommonActor* ActorDataPool::fromPool(CommonActorBasicSpawnData& spawnData
										 , const bool forceTo /*= false*/
										 , const ActorKey& actorKey/* = ActorKey()*/) noexcept
	{
		uint32 targetIndex = 0;
		if (true == forceTo) // 클라에서는 서버에서 내려주는 그대로 사용한다. 코드 바꿔야할듯..
		{
			targetIndex = actorKey.getKeyValue();
		}
		else
		{
			LockableObject* targetFreeIndexesLock = nullptr;
			std::vector<uint32>* targetFreeIndexes = nullptr;
			if (false == getFreeIndexesAndLock(spawnData._actorType, targetFreeIndexesLock, targetFreeIndexes))
			{
				TA_ASSERT_DEV(false, "비정상");
				return nullptr;
			}

			// 잠깐 걸고 인덱스만 빼오는거라 괜찮을듯
			ScopedLock lock(targetFreeIndexesLock);

			if (true == targetFreeIndexes->empty())
			{
				TA_ASSERT_DEV(false, "풀이 가득찼습니다 대책을 마련해야합니다. ActorType : %d", static_cast<uint8>(spawnData._actorType));
				return nullptr;
			}
			else
			{
				targetIndex = targetFreeIndexes->back();
				targetFreeIndexes->pop_back();
			}
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
			LockableObject* targetFreeIndexesLock = nullptr;
			std::vector<uint32>* targetFreeIndexes = nullptr;
			if (false == getFreeIndexesAndLock(getActorType(actorKey), targetFreeIndexesLock, targetFreeIndexes))
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

			ScopedLock lock(targetFreeIndexesLock);
			targetFreeIndexes->push_back(index);
		}

		return true;
	}

	const bool ActorDataPool::isFull(const ActorType& actorType) noexcept
	{
		LockableObject* targetFreeIndexesLock = nullptr;
		std::vector<uint32>* targetFreeIndexes = nullptr;
		if (false == getFreeIndexesAndLock(actorType, targetFreeIndexesLock, targetFreeIndexes))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}
		
		ScopedLock lock(targetFreeIndexesLock, true);
		return (false == targetFreeIndexes->empty());
	}

	void ActorDataPool::logTest(void) noexcept
	{
		//uint32 freeIndexCount = 0;
		//{
		//	ScopedLock lock(this, true);
		//	freeIndexCount = _freeIndex.size();
		//}

		//TA_LOG_DEV("FreeIndex Count : %d\n\n", freeIndexCount);

		//bool rv = false;
		//for (uint32 index = 0; index < _maxCount; ++index)
		//{
		//	CommonActor* actor = getActor(index);
		//	{
		//		ScopedLock lock(actor, true);
		//		rv = actor->isActive_();
		//	}

		//	TA_LOG_DEV("ActorKey : %d is %d , ", index, rv);
		//}
	}

	ActorType ActorDataPool::getActorType(const ActorKey& actorKey) noexcept
	{
		ActorType actorType = ActorType::Count;
		uint32 relativeGroupIndex = 0;
		if (false == getRelativeGroupIndexAndActorType(actorKey, actorType, relativeGroupIndex))
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return ActorType::Count;
		}

		return actorType;
	}

	const bool ActorDataPool::getRelativeGroupIndex(const ActorKey& actorKey, uint32& relativeGroupIndex) const noexcept
	{
		ActorType actorType = ActorType::Count;
		return getRelativeGroupIndexAndActorType(actorKey, actorType, relativeGroupIndex);
	}

	const bool ActorDataPool::getRelativeGroupIndexAndActorType(const ActorKey& actorKey, ActorType& actorType, uint32& relativeGroupIndex) const noexcept
	{
		uint32 actorKeyValue = actorKey.getKeyValue();

		uint32 finalBoundary = 0;
		ActorType finalActorType = ActorType::Count;
		bool isValid = false;
		for (std::unordered_map<ActorType, ActorGroup>::const_iterator it = ActorDataGroups.begin(); ActorDataGroups.end() != it; ++it)
		{
			// 0이 나오는 이유는 그 아래에 존재한다는 것이다.
			// 이 액터의 ActorType와 Boundary는 이전에 설정해둔 ActorType과 Boundary다 => ActorType와 Boundary를 갱신하지 않고 빠져나온다.
			if (0 == (actorKeyValue / (finalBoundary + it->second._count)))
			{
				isValid = true;
				finalActorType = it->first;
				break;
			}

			finalBoundary += it->second._count;
		}

		if (false == isValid)
		{
			TA_ASSERT_DEV(false, "비정상적인 액터키입니다.");
			return false;
		}

		relativeGroupIndex = actorKeyValue - finalBoundary;
		actorType = finalActorType;

		return true;
	}

	bool ActorDataPool::initializeAllComponentCountFromActorType(void) noexcept
	{
#define INITIALIZE_COMPONENT_COUNT(OwnerActorType)																				\
		{																														\
			std::unordered_map<ActorType, ActorGroup>::const_iterator it = ActorDataGroups.find(OwnerActorType);				\
			if (ActorDataGroups.end() == it)																					\
			{																													\
				TA_ASSERT_DEV(false, "비정상");																					\
				return false;																									\
			}																													\
			const std::vector<ActorComponentType>& componentTypeList = it->second._componentTypeList;							\
			const uint32 componentTypeCount = componentTypeList.size();															\
			for (uint32 index = 0; index < componentTypeCount; ++index)															\
			{																													\
				if (false == addComponentCountFromActorType(OwnerActorType, componentTypeList[index], it->second._count))		\
				{																												\
					TA_ASSERT_DEV(false, "비정상");																				\
					return false;																								\
				}																												\
			}																													\
		}																														\


		INITIALIZE_COMPONENT_COUNT(ActorType::Player)
		INITIALIZE_COMPONENT_COUNT(ActorType::Npc)
		INITIALIZE_COMPONENT_COUNT(ActorType::Object)

		TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");

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
				ADD_COMPONENT_COUNT(Object, _objectComponentPool)

				TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");

#undef INITIALIZE_COMPONENT_COUNT

		default:
			break;

		}

		return true;
	}

	bool ActorDataPool::getFreeIndexesAndLock(const ActorType& actorType, LockableObject*& freeIndexesLock, std::vector<uint32>*& freeIndexes) noexcept
	{
		switch (actorType)
		{
		case ActorType::Player:
			{
				freeIndexes = &_playerFreeIndexes;
				freeIndexesLock = &_playerFreeIndexesLock;
			}
			break;
		case ActorType::Npc:
			{
				freeIndexes = &_npcFreeIndexes;
				freeIndexesLock = &_npcFreeIndexesLock;
			}
			break;
		case ActorType::Object:
			{
				freeIndexes = &_objectFreeIndexes;
				freeIndexesLock = &_objectFreeIndexesLock;
			}
			break;
		default:
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
			break;

			TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 확인해주세요");
		}

		return true;
	}
}

