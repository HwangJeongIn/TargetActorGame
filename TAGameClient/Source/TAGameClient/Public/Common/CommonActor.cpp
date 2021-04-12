#include "Common/CommonActor.h"
#include "Common/ActorComponent.h"
#include "Common/Socket.h"
#include "Common/ScopedLock.h"
#include "Common/ComponentData.h"
#include "Common/ActorDataGroups.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	CommonActor::CommonActor(void) noexcept
		: _socket(nullptr)
		, _actorType(ActorType::Default)
		, _networkOwner(nullptr)
		, _countOfComponentsToInitialize(-1)
	{
	}

	CommonActor::~CommonActor(void) noexcept
	{
		if (nullptr != _socket)
		{
			delete _socket;
		}
	}

	Socket* CommonActor::getNetwork_(void) noexcept
	{
		// 일단 소켓은 한번연결되면 연결이 끊어질때까지 삭제되지 않고,
		// 소켓 멤버에 영향을 미치는 함수는 동작이 차례대로 이루어지기 때문에 // recv 따로 락을 안걸었다.
		// send같은 경우 동적할당해서 동기화 안해도된다.
		if (nullptr == _socket)
		{
			if (nullptr != _networkOwner)
			{
				return _networkOwner->getNetwork_();
			}
		}
		return _socket;
	}

	const ActorKey& CommonActor::getActorKey(void) const noexcept
	{
		return _actorKey;
	}

	void CommonActor::setActorKey_(const ActorKey& actorKey) noexcept
	{
		_actorKey = actorKey;
	}

	ActorType CommonActor::getActorType_(void) noexcept
	{
		return _actorType;
	}

	ActorType CommonActor::getActorType(void) noexcept
	{
		ScopedLock lock(this, true);

		return _actorType;
	}

	bool CommonActor::isPlayer_(void) const noexcept
	{
		return ((ActorType::Player == _actorType) || (ActorType::ControlledByPlayer == _actorType));
	}

	bool CommonActor::isPlayer(void) noexcept
	{
		ScopedLock lock(this);
	
		return (isPlayer_());
	}

	bool CommonActor::isMainPlayer_(void) const noexcept
	{
		return (ActorType::Player == _actorType);
	}

	bool CommonActor::isMainPlayer(void) noexcept
	{
		ScopedLock lock(this);

		return (isMainPlayer_());
	}

	bool CommonActor::hasNetwork(void) noexcept
	{
		{
			ScopedLock lock(this, true);
			if (nullptr != _socket)
			{
				return true;
			}
		}

		if (nullptr != _networkOwner)
		{
			return _networkOwner->hasNetwork();
		}

		return false;
	}

	//bool CommonActor::canCommunicate_(void) const noexcept
	//{
	//	return (nullptr != _networkOwner);
	//}

	//bool CommonActor::canCommunicate(void) noexcept
	//{
	//	ScopedLock lock(this);

	//	return (canCommunicate_());
	//}

	void CommonActor::onToPool(void) noexcept
	{
		// 무조건 모든정보가 한꺼번에 내려가면서 한꺼번에 처리되어야한다.
		//std::unordered_map<ActorComponentType, ActorComponent&>::const_iterator it = _actorComponents.begin();
		//
		//for (; it != _actorComponents.end(); ++it)
		//{
		//	{
		//		ScopedLock componentLock(&(it->second));

		//		// 어차피 정해져있는데 안해도되겠다
		//		//it->second.setOwner_(nullptr);
		//		it->second.setIsValid_(false);
		//		it->second.updateToPool_();
		//	}

		//	it->second.onToPool();
		//}	

		const uint32 count = static_cast<uint32>(ActorComponentType::Count);
		for (uint32 index = 0; index < count; ++index)
		{
			ActorComponent* actorComponent = GetActorComponent(this, static_cast<ActorComponentType>(index));
			if (nullptr == actorComponent) // 실제 이 이 액터에 맞는 컴포넌트는 있지만 valid한 상태가 아니다
			{
				continue;
			}

			{
				ScopedLock componentLock(actorComponent);
				actorComponent->setComponentValid_(false);
				actorComponent->updateToPool_();
			}

			actorComponent->onToPool();
		}

		{
			ScopedLock lock(this);
			//_actorComponents.clear();
			_countOfComponentsToInitialize = -1;
			_actorType = ActorType::Default;
			updateToPool_();
		}
	}

	void CommonActor::onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept
	{
		// 초기화도 단계를 나눠야 할듯.. 
		// 중간에 스레드가 들어와서 초기화되지않은 정보 사용하거나 다른 스레드가 같은 Pool value사용할 수 있음
		// 모든 것들이 초기화가 정확히 완료된 시점이 필요하다.
		/*
		1. 막 꺼내서 초기화하는 단계 // 풀에서 사용될순 없지만 그렇다고 다른곳에서 접근해서도 안됨
		   현재 본인은 초기화가 완료되었지만 다른 컴포넌트나 리소스는 초기화 안되었을 수 도 있다.
		
		2. 꺼낸후 초기화 완료된 상태
		   모든것이 초기화가 완료되었다. 어떤것이든 사용해도된다.

		Waiting > Initializing	=> onFromPool함수 호출 // 주로 개별 컴포넌트 초기화
		Initializing > Using	=> onActive함수 호출 // 해당 액터의 컴포넌트 접근 가능
		Using > Waiting			=> onToPool함수 호출
		*/

		const std::vector<ActorComponent*>& componentList = spawnData._componentList;
		const uint32 count = componentList.size();

		// 무조건 모든 정보가 초기화 되고 사용되어야 한다.
		{
			ScopedLock lock(this);

			updatePoolValueState_(PoolValueState::Initializing);
			//for (uint32 index = 0; index < count; ++index)
			//{
			//	componentList[index]->setComponentValid_(true);
			//	//addComponent_(componentList[index]);
			//}
			_countOfComponentsToInitialize = count;

			_actorType = spawnData._actorType;
			//if (nullptr != _socket)
			//{
			//	delete _socket;
			//	_socket = nullptr;
			//}

			switch (spawnData._networkOwnerType)
			{
			case NetworkOwnerType::None:
				{
					__noop;
				}
				break;
			case NetworkOwnerType::Mine:
				{
					initializeNetwork_();
				}
				break;
			case NetworkOwnerType::BorrowingNetwork:
				{
					borrowNetwork_();
				}
				break;
			default:
				{

				}
				break;
			}
		}

		for (uint32 index = 0; index < count; ++index)
		{
			{
				ScopedLock componentLock(componentList[index]);
				componentList[index]->setOwner_(this);
				componentList[index]->updatePoolValueState_(PoolValueState::Initializing);
			}

			componentList[index]->onFromPool();
		}
	}

	void CommonActor::onActive(void) noexcept
	{
		// 이제부터 서로 접근 가능
		{
			ScopedLock lock(this);
			updateFromPool_(_actorKey.getKeyValue());
		}

		//std::unordered_map<ActorComponentType, ActorComponent&>::iterator it = _actorComponents.begin();
		//for (; it != _actorComponents.end(); ++it)
		//{
		//	{
		//		ScopedLock componentLock(&(it->second));
		//		it->second.updateFromPool_(_actorKey.getKeyValue());
		//	}

		//	it->second.onActive();
		//}

		const uint32 count = static_cast<uint32>(ActorComponentType::Count);
		for (uint32 index = 0; index < count; ++index)
		{
			ActorComponent* actorComponent = GetActorComponent(this, static_cast<ActorComponentType>(index));
			if (nullptr == actorComponent) // 실제 이 이 액터에 맞는 컴포넌트는 있지만 valid한 상태가 아니다
			{
				continue;
			}

			{
				ScopedLock componentLock(actorComponent);
				actorComponent->updateFromPool_(_actorKey.getKeyValue());
			}

			actorComponent->onActive();
		}
	}

	const int32 CommonActor::getCountOfComponentsToInitialize_(void) const noexcept
	{
		return _countOfComponentsToInitialize;
	}
	
	bool CommonActor::decreaseCountOfComponentsToInitialize(void) noexcept
	{
		bool rv = false;
		{		
			ScopedLock lock(this);
			if (0 >= _countOfComponentsToInitialize)
			{
				TA_ASSERT_DEV(false, "비정상입니다.")
				return false;
			}
			--_countOfComponentsToInitialize;
			rv = (0 == _countOfComponentsToInitialize);
		}

		if(true == rv)
		{
			// 락걸고 들어가면 제한이 많을듯 
			// 중간에 들어오면? 그래도 데이터 접근은 내부에서 atomic하게 이루어지고 액터와 컴포넌트는 풀에 반납 될 뿐 실제로 삭제되지않는다.
			onActive();
		}

		return true;
	}

	//bool CommonActor::addComponent_(ActorComponent* actorComponent) noexcept
	//{
	//	const ActorComponentType actorComponentType = actorComponent->getActorComponentType();
	//	if (nullptr == actorComponent)
	//	{
	//		TA_ASSERT_DEV(false, "생성에 실패했습니다. type : %d", actorComponentType)
	//			return false;
	//	}

	//	std::unordered_map<ActorComponentType, ActorComponent&>::const_iterator target = _actorComponents.find(actorComponentType);
	//	if (_actorComponents.end() != target)
	//	{
	//		TA_ASSERT_DEV(false, "두번 추가하려 합니다.")
	//			return false;
	//	}

	//	std::pair<ActorComponentType, ActorComponent&> pair(actorComponentType, *actorComponent);
	//	_actorComponents.insert(pair);
	//	return true;
	//}

	//ActorComponent* CommonActor::getComponent_(const ActorComponentType type) noexcept
	//{
	//	std::unordered_map<ActorComponentType, ActorComponent&>::const_iterator target = _actorComponents.find(type);
	//	if (_actorComponents.end() == target)
	//	{
	//		TA_ASSERT_DEV(false, "존재하지 않습니다. type : %d", type)
	//			return nullptr;
	//	}

	//	return &(target->second);
	//}

	bool CommonActor::borrowNetwork_(void) noexcept
	{
		_networkOwner = g_app->getActorManager()->getOwnerActor();
		return true;
	}

	bool CommonActor::initializeNetwork_(void) noexcept
	{
		if (nullptr != _socket)
		{
			delete _socket;
		}

		_socket = new Socket(SocketType::Tcp);

		return true;
	}
}


namespace ta
{
	CommonActorBasicSpawnData::CommonActorBasicSpawnData(void) noexcept
		: _networkOwnerType(NetworkOwnerType::None)
		, _actorType(ActorType::Default)
	{
	}

	CommonActorBasicSpawnData::~CommonActorBasicSpawnData(void) noexcept
	{
		_componentTypeList.clear();
		_componentList.clear();
	}

	void CommonActorBasicSpawnData::initializeComponents(const std::vector<ActorComponentType>& componentTypeList, const ActorType& actorType) noexcept
	{
		_componentTypeList = componentTypeList;
		_actorType = actorType;
	}

	void CommonActorBasicSpawnData::initializeComponentsAsDefaultActor(void) noexcept
	{
		initializeComponents(DefaultActor, ActorType::Default);
	}

	void CommonActorBasicSpawnData::initializeComponentsAsOwner(void) noexcept
	{
		initializeComponents(OwnerActor, ActorType::Owner);
	}

	void CommonActorBasicSpawnData::initializeComponentsAsPlayer(void) noexcept
	{
		initializeComponents(PlayerActor, ActorType::Player);
	}

	void CommonActorBasicSpawnData::initializeComponentsAsControlledByPlayer(void) noexcept
	{
		initializeComponents(PlayerActor, ActorType::ControlledByPlayer);
	}

	void CommonActorBasicSpawnData::initializeComponentsAsMonster(void) noexcept
	{
		initializeComponents(MonsterActor, ActorType::Monster);
	}

	void CommonActorBasicSpawnData::initializeComponentsAsNpc(void) noexcept
	{
		initializeComponents(NpcActor, ActorType::Npc);
	}
}


namespace ta
{
	CommonActorDetailSpawnData::CommonActorDetailSpawnData(const Vector& position, const Vector& rotation, const GroupGameDataKey& groupKey) noexcept
		: _position(position)
		, _rotation(rotation)
		, _groupGameDataKey(groupKey)
	{

	}

	CommonActorDetailSpawnData::~CommonActorDetailSpawnData(void) noexcept
	{
	}

	bool CommonActorDetailSpawnData::isUser(void) const noexcept
	{
		return false;
	}
}


namespace ta
{
	CommonActorDetailSpawnDataForUser::CommonActorDetailSpawnDataForUser(const Vector& position, const Vector& rotation, const GroupGameDataKey& groupKey) noexcept
		: CommonActorDetailSpawnData(position, rotation, groupKey)
	{
	}

	CommonActorDetailSpawnDataForUser::~CommonActorDetailSpawnDataForUser(void) noexcept
	{
	}

	bool CommonActorDetailSpawnDataForUser::isUser(void) const noexcept
	{
		return true;
	}
}