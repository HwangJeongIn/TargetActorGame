#include "Client/ClientMoveActorComponent.h"
#include "Client/ClientMoveActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"



namespace ta
{
	ClientMoveActorComponent::ClientMoveActorComponent(void) noexcept
	{
	}

	ClientMoveActorComponent::~ClientMoveActorComponent(void) noexcept
	{
	}

	void ClientMoveActorComponent::onActive(void) noexcept
	{
		CommonMoveActorComponent::onActive();

		ClientMoveActorSystem* moveActorSystem = GetActorSystem<ClientMoveActorSystem>();
		if (false == moveActorSystem->setStartTransform(getOwner(), _currentPosition, _currentRotation))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}
}

