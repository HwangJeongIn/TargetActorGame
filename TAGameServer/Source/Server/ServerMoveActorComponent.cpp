#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerMoveActorSystem.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	ServerMoveActorComponent::ServerMoveActorComponent(void) noexcept
	{
	}

	ServerMoveActorComponent::~ServerMoveActorComponent(void) noexcept
	{
	}

	void ServerMoveActorComponent::onActive(void) noexcept
	{
		CommonMoveActorComponent::onActive();

		Vector position;
		{
			ScopedLock lock(this, true);
			position = getCurrentPosition_();
		}

		ServerMoveActorSystem* moveActorSystem = GetActorSystem<ServerMoveActorSystem>();
		// 해당 클라의 플레이어가 최초로 스폰되는경우 해당서버에서 내려준데이터로 움직이게된다. 
		if (false == moveActorSystem->moveActorAndNotify(getOwner(), position, true, false)) // 클라는 스폰되면서 onActive에서 위치가 갱신될것이다.
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}

	}

	const std::unordered_set<ActorKey>& ServerMoveActorComponent::getViewList_(void) const noexcept
	{
		return _viewList;
	}

	void ServerMoveActorComponent::setViewList_(const std::unordered_set<ActorKey>& newViewList) noexcept
	{
		_viewList = newViewList;
	}

	bool ServerMoveActorComponent::addToViewList_(const ActorKey& target) noexcept
	{
		std::pair<std::unordered_set<ActorKey>::iterator, bool> rv = _viewList.insert(target);
		return rv.second;
	}

	bool ServerMoveActorComponent::removeFromViewList_(const ActorKey& target) noexcept
	{
		size_t rv = _viewList.erase(target);
		return (0 < rv);
	}

	bool ServerMoveActorComponent::checkExistInViewList_(const ActorKey& target) const noexcept
	{
		std::unordered_set<ActorKey>::iterator rv = _viewList.find(target);
		return (_viewList.end() != rv);
	}
}

