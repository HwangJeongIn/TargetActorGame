#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include <vector>


namespace ta
{
	class CommonActor;
	class ActorDataPool;
	class ActorKey;
	class CommonActorBasicSpawnData;
	class ComponentData;
	class ActorComponent;
	class Sector;
	class Sectors;
	class SectorKey;
}


namespace ta
{
	class CommonActorManager : public Uncopyable
	{
	public:
		virtual bool initialize(void) noexcept;
		bool doInitialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		CommonActor* getOwnerActor(void) noexcept;
		const ActorKey getOwnerActorKey(void) const noexcept;
		CommonActor* getActor(const ActorKey& actorKey, bool activeActorOnly = false) noexcept;
		ActorComponent* getActorComponent(const ActorKey& actorKey, const ActorComponentType componentType) noexcept;
		//ActorComponent* getActorComponents(const ActorComponentType componentType) noexcept;
		Sector* getSector(const SectorKey& sectorKey) noexcept;

		// 서버 클라에서 오너액터 생성할때 사용
		CommonActor* createOwnerActor(void) noexcept;

		// DB작업될때까지 액터 컴포넌트 데이터 초기화는 이함수로 대체
		// 이제 거의 쓸일이 없다. 스폰데이터로 스폰하는 기능 추가 / 임시 fake DB 코드사용중
		//bool initializeDefaultActorFixedComponents(const ActorKey& actorKey) noexcept;
		bool initializeOwnerFixedComponents(const ActorKey& actorKey) noexcept;
		//bool initializePlayerFixedComponents(const ActorKey& actorKey) noexcept;
		//bool initializeMonsterFixedComponents(const ActorKey& actorKey) noexcept;
		ComponentData* createFixedComponentData(const ActorComponentType& actorComponent) noexcept;
		
		// DB작업될때까지 액터 컴포넌트 데이터 초기화는 이함수로 대체

		bool initializeActorComponent(const ActorKey& actorKey, const ComponentData* data, const bool needToBeDeleted = true) noexcept;

		bool destroyActor(const ActorKey& actorKey) noexcept;
		bool destroyActor(const CommonActor* actor) noexcept;

		void logTest(void) noexcept;

		virtual ~CommonActorManager(void) noexcept;
	protected:
		CommonActorManager(void) noexcept;

	private:
		// DB작업될때까지 액터 컴포넌트 데이터 초기화는 이함수로 대체
		bool initializeFixedComponents(const ActorKey& actorKey, const std::vector<ActorComponentType>& typeList) noexcept;
		// DB작업될때까지 액터 컴포넌트 데이터 초기화는 이함수로 대체

	protected:
		
		Sectors* _sectors;
		
		// NOTE : 만들어지는 리소스는 풀에 있는 값으로 정적으로 할당된 값이다. 무조건 CommonActorManager를 통해서 생성, 파괴한다.
		// 액터키는 액터풀, 컴포넌트 풀의 인덱스로 정의하자
		// 만약에 해당액터에 해당 컴포넌트가 필요없다고 해도 일단 할당해놓고 사용하지말자
		// 따로 관리 안하고(unordered_map) 쉽게 인덱스로 접근하기 위함이다.

		ActorDataPool* _actorDataPool;
	};																																	 

}
