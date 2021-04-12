#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/ActorDataPoolValue.h"
#include "Common/Lockable.h"
#include "Common/Uncopyable.h"
#include "Common/Vector.h"
#include <unordered_map>


namespace ta
{
	class ActorComponent;
	class ActorNetwork;
	class ActorDataPool;
	class CommonActorBasicSpawnData;
	class Socket;
	class ComponentData;
}


namespace ta
{
	class CommonActor : public Uncopyable, public Lockable, public ActorDataPoolValue
	{
		//friend class ActorDataPool;
	public:
		// TODO : 액터 매니저에서 관리, 액터 스폰정보 정의, 액터 스폰정보 -> 액터 정보 구현
		CommonActor(void) noexcept;
		virtual ~CommonActor(void) noexcept;

		Socket* getNetwork_(void) noexcept;

		const ActorKey& getActorKey(void) const noexcept;
		void setActorKey_(const ActorKey& actorKey) noexcept;
		//ActorType getActorType_(void) noexcept;
		ActorType getActorType(void) noexcept;
		void setActorType_(const ActorType& actorType) noexcept;
		
		// 플레이어라면 항상 통신할 수 있음
		bool isPlayer_(void) const noexcept;
		bool isPlayer(void) noexcept;

		bool isMainPlayer_(void) const noexcept;
		bool isMainPlayer(void) noexcept;

		bool hasNetwork(void) noexcept;
		//ActorComponent* getComponent_(const ActorComponentType type) noexcept;

		/*
		예를들어 몬스터인경우 
		1. 플레이어가 주위 섹터에 없으면 몬스터를 서버에서만 업데이트 돌린다. 
		2. 그러다가 클라가 이동해서 해당 섹터에 들어가면 서버레벨에서 해당 몬스터를 스폰하라고 클라레벨로 보내준다.
		3. 플레이어가 데미지를 입히는 행위 등은 해당 클라레벨의 소켓을 통해서 서버레벨로 보낸다.
		4. 만약 플레이어가 해당 섹터를 벗어난다면, 서버레벨에서 새롭게 들어간 섹터에 대한 다른 액터들 스폰 언스폰을 클라레벨로 내려준다.
		*/

		// NPC인데 개별적으로 통신하고 싶을때(주변 클라에 뿌려주는 역할이 아니고 혼자 통신하고싶을 때) networkOwner가 nullptr이 아닌경우에만 클라 <=> 서버 통신가능 
		// 그전까지는 서버에서만 돌아야한다. 
		// 최초로 몬스터가 클라에 스폰될때 _networkOwner을 설정하고 서버레벨과 통신한다.

		// 클라에만 존재하는 액터(데코레이션 액터 등) _socket / _networkOwner가 nullptr이다.
		// 서버에만 존재하는 액터 역시 _socket / _networkOwner가 nullptr이다. 클라로 내려줘야할떄는 해당섹터 안에있는 클라를 모아서 각 클라레벨로 패킷을 쏜다.
		//bool canCommunicate_(void) const noexcept;
		//bool canCommunicate(void) noexcept;

		virtual void onToPool(void) noexcept;
		virtual void onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept;
		virtual void onActive(void) noexcept;

		const int32 getCountOfComponentsToInitialize_(void) const noexcept;
		bool decreaseCountOfComponentsToInitialize(void) noexcept;

		
	protected:
		// CommonActorManager, CommonActor내에서만 사용 
		// 굳이 따로 관리해야할까 싶어서 주석처리 // 그냥 인덱스로 풀 뒤져서 가지고오자 락안걸어도된다.
		//bool addComponent_(ActorComponent* actorComponent) noexcept;

	private:
		bool borrowNetwork_(void) noexcept;
		bool initializeNetwork_(void) noexcept;

	protected:
		ActorKey _actorKey;
		ActorType _actorType;

		// 굳이 따로 관리해야할까 싶어서 주석처리 // 그냥 인덱스로 풀 뒤져서 가지고오자 락안걸어도된다.
		//std::unordered_map<ActorComponentType, ActorComponent&> _actorComponents;

		// 처음 초기화 -1 / 컴포넌트 추가될때 컴포넌트 갯수로 초기화 / 컴포넌트가 모두 초기화되었을때 0이되고 onActive가 불린다.
		int32 _countOfComponentsToInitialize;
		//ActorNetwork* _actorNetwork;

		// 클라는 서버로 통하는 소켓이 하나만 존재한다. // 최초에 하나만 연결하고 나머지 캐릭터가 그 하나가지고 공유해서 사용한다.
		// 서버는 여러개의 소켓이 존재한다.
		CommonActor* _networkOwner;
		// 플레이어인 경우에만 nullptr이 아니다.
		Socket* _socket;
	};
}


namespace ta
{
	class CommonActorBasicSpawnData
	{
	public:
		CommonActorBasicSpawnData(void) noexcept;
		~CommonActorBasicSpawnData(void) noexcept;

		void initializeComponents(const std::vector<ActorComponentType>& componentTypeList, const ActorType& actorType) noexcept;

		//void initializeComponentsAsDefaultActor(void) noexcept;
		void initializeComponentsAsOwner(void) noexcept;
		void initializeComponentsAsPlayer(void) noexcept;
		void initializeComponentsAsControlledByPlayer(void) noexcept;

		//void initializeComponentsAsMonster(void) noexcept;
		void initializeComponentsAsNpc(void) noexcept;
		void initializeComponentsAsObject(void) noexcept;
	public:
		// 풀에서 가지고 올때 사용하고 액터안에서 초기화할때는 사용되지 않는다.
		std::vector<ActorComponentType> _componentTypeList;
		// 풀에서 초기화시켜주면 아랫 변수에 넣어주면 액터안에서 해당 컴포넌트 포인터를 가지고 초기화
		std::vector<ActorComponent*> _componentList;

		// 이플래그가 켜져있으면 통신을 위한 소켓도 같이 생성한다.
		// NPC의 경우 서버에서만 돌고 있다가 클라에 보이는 시점에 플레이어의 소켓을 통해서 업데이트 된다.
		NetworkOwnerType _networkOwnerType;
		ActorType _actorType;

		// TODO : 컴포넌트당 정보가 필요하기 때문에 추후에 넣어줘야 한다 => ComponentData로 하나씩
	};
}

// 데이터로부터 로드할때 사용 // location, rotation, ComponentDataSet(GroupGameDataKey로 추출)
namespace ta
{
	class CommonActorDetailSpawnData
	{
	public:
		explicit CommonActorDetailSpawnData(const Vector& position, const Vector& rotation, const GroupGameDataKey& groupKey) noexcept;
		virtual ~CommonActorDetailSpawnData(void) noexcept;

		virtual bool isUser(void) const noexcept;

	public:
		Vector _position;
		Vector _rotation;
		GroupGameDataKey _groupGameDataKey;
	};
}


namespace ta
{
	class CommonActorDetailSpawnDataForUser : public CommonActorDetailSpawnData
	{
	public:
		explicit CommonActorDetailSpawnDataForUser(const Vector& position, const Vector& rotation, const GroupGameDataKey& groupKey) noexcept;
		virtual ~CommonActorDetailSpawnDataForUser(void) noexcept;

		virtual bool isUser(void) const noexcept override final;

	public:
		// DB에서 받은정보가 있으면 추가
	};
}