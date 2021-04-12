#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonActor.h"
#include "Common/Uncopyable.h"
#include "Common/Lockable.h"
#include <vector>


/*
링크드리스트가 아닌 배열형식으로 가지고 있게 한다.
링크드리스트(list)로 할당하면 연속적인 메모리 값을 가지지 않을 확률이 높고, 그러면 지역성이 떨어질 가능성이 높다.
가변배열(vector)을 사용하면, push_back을 할때나 reserve를 할때 relocate되기 때문에 해당 배열의 객체를 캐싱하고 있는 곳에서 문제가 생긴다.
캐싱하고 있는 곳에 문제가 없게 하려면 relocate될때마다 해당 객체를 캐싱하고 있는 곳에가서 모두 수정해줘야한다.

아래의 해결방법으로 고민중이다.
1. 일단 메모리를 조금 먹더라도 최대한 큰 크기로 만들어 두는 것으로 진행한다.
2. 고정크기 배열을 할당해두고 만약에 너무많아서 확장해야할 경우에 해당 배열을 가진 풀을 하나 더 추가한다.


1의 경우는 쓸데없이 메모리를 많이 먹게된다는 점이 문제고, 예상보다 큰 수가 들어오면 일부를 사용할 수 없다.

2의 경우는 각 고정크기 배열 그룹간 메모리가 연속적이지 않기 때문에 지역성이 떨어질 가능성이 높다. 
그리고 풀에 반납되는게 많아졌을때 다시 풀을 축소시켜야하는데 그 기준을 잡는것도 중요하다.

일단 메모리가 넉넉하니 1번의 방법을 사용해서 구현해두자 
풀크기를 5만정도로 잡아두면 메모리는 5만 * 객체당 메모리(30-40바이트정도?) * 객체의 종류(15종이라고 하자) = 22,500,000 바이트 = 22.5메가밖에 안된다
*/

namespace ta
{
	class CommonActor;
	class CommonActorBasicSpawnData;
	class ActorKey;
	class CommonMoveActorComponent;
	class CommonActionActorComponent;
	class CommonAiActorComponent;
	class CommonCharacterActorComponent;
	class CommonInventoryActorComponent;
	class ActorComponentPool;
}



namespace ta
{
	class ActorDataPool : public Uncopyable
	{
	public:
		ActorDataPool(void) noexcept;
		virtual ~ActorDataPool(void) noexcept;

		virtual bool initialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		// 클라에서는 서버에서 내려주는거 그냥 생성해야해서 freeIndex무시한다.
		CommonActor* fromPool(const ActorKey& targetActorKey, CommonActorBasicSpawnData& spawnData) noexcept;
		// 서버에서는 직접 빈곳을 찾아서 생성하고 클라로 내려줘야한다.
		CommonActor* fromPool(CommonActorBasicSpawnData& spawnData, const bool forceTo = false, const ActorKey& actorKey = ActorKey()) noexcept;
		bool initializeActorComponent(const ActorKey& actorKey, const ComponentData* data, const bool needToBeDeleted) noexcept;
		bool toPool(const ActorKey& actorKey, const bool forceTo = false) noexcept;
		const bool isFull(void) noexcept;


		virtual CommonActor* getActor(const ActorKey& actorKey) noexcept = 0;

		uint32 getMaxCount(void) const noexcept;

		virtual ActorComponent* getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept = 0;


		//template <typename T>
		//T* getActorComponent(const ActorKey& actorKey, const ActorComponentType& componentType) noexcept
		//{
		//	getActorType();
		//	std::unordered_map<ActorType, ActorGroup>::const_iterator ActorGroup = ActorDataGroups.find(targetActorType);
		//	if (ActorDataGroups.end() == ActorGroup)
		//	{
		//		TA_ASSERT_DEV(false, "비정상");
		//		return false;
		//	}

		//	if (ActorGroup->second._count <= relativeGroupIndex)
		//	{
		//		TA_ASSERT_DEV(false, "비정상적인 인덱스 입니다. ActorType : %d, Index : %d", static_cast<uint8>(targetActorType), relativeGroupIndex);
		//		return false;
		//	}

		//	std::unordered_map<ActorType, uint32>::const_iterator it = _startIndexMap.find(targetActorType);
		//	if (_startIndexMap.end() == it)
		//	{
		//		TA_ASSERT_DEV(false, "비정상");
		//		return false;
		//	}

		//	return static_cast<T*>(_actorComponents[it->second + relativeGroupIndex]);
		//}

		void logTest(void) noexcept;

		ActorType getActorType(const ActorKey& actorKey) noexcept;

	protected:
		const bool getRelativeGroupIndex(const ActorKey& actorKey, uint32& relativeGroupIndex) const noexcept;
		const bool getRelativeGroupIndexAndActorType(const ActorKey& actorKey, ActorType& actorType, uint32& relativeGroupIndex) const noexcept;

	private:
		bool initializeAllComponentCountFromActorType(void) noexcept;
		bool addComponentCountFromActorType(const ActorType& ownerActorType, const ActorComponentType& actorComponentType, const uint32& count) noexcept;

	protected:
		

		LockableObject _playerFreeIndexesLock;
		std::vector<uint32> _playerFreeIndexes;

		LockableObject _npcFreeIndexesLock;
		std::vector<uint32> _npcFreeIndexes;

		LockableObject _objectFreeIndexesLock;
		std::vector<uint32> _objectFreeIndexes;

		// CommonActor인덱스와 Component 인덱스가 같다. 현재 사용하고 있는지는 CommonActor의 값을 보고 판단한다.
		// 인덱스로 접근하면 안되네..
		CommonActor* _actorPoolValues;

		// ** 컴포넌트 관련 추가
		ActorComponentPool* _moveComponentPool;
		ActorComponentPool* _actionComponentPool;
		ActorComponentPool* _aiComponentPool;
		ActorComponentPool* _characterComponentPool;
		ActorComponentPool* _inventoryComponentPool;

		//CommonMoveActorComponent*				_moveComponentPoolValues;
		//uint32									_moveComponentPoolValuesCount;
		//std::unordered_map<ActorType, uint32>	_moveComponentPoolValuesStartIndexMap;
		//
		//CommonActionActorComponent*				_actionComponentPoolValues;
		//uint32									_actionComponentPoolValuesCount;
		//std::unordered_map<ActorType, uint32>	_actionComponentPoolValuesStartIndexMap;
		//
		//CommonAiActorComponent*					_aiComponentPoolValues;
		//uint32									_aiComponentPoolValuesCount;
		//std::unordered_map<ActorType, uint32>	_aiComponentPoolValuesStartIndexMap;
		//
		//CommonCharacterActorComponent*			_characterComponentPoolValues;
		//uint32									_characterComponentPoolValuesCount;
		//std::unordered_map<ActorType, uint32>	_characterComponentPoolValuesStartIndexMap;
		//
		//CommonInventoryActorComponent*			_inventoryComponentPoolValues;
		//uint32									_inventoryComponentPoolValuesCount;
		//std::unordered_map<ActorType, uint32>	_inventoryComponentPoolValuesStartIndexMap;

		TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
	};
}
