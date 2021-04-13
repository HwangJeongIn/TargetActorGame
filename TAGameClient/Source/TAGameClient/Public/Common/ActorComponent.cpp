#include "Common/ActorComponent.h"
#include "Common/CommonActor.h"


namespace ta
{
	ActorComponent::ActorComponent(const ActorComponentType& actorComponentType) noexcept
		: _actorComponentType(actorComponentType)
		, _componentValid(false)
	{
	}

	ActorComponent::~ActorComponent(void) noexcept
	{
	}

	const ActorComponentType ActorComponent::getActorComponentType(void) const noexcept
	{
		return _actorComponentType;
	}
	
	CommonActor* ActorComponent::getOwner(void) noexcept
	{
		return _owner;
	}
	
	const ActorKey& ActorComponent::getOwnerActorKey(void) noexcept
	{
		return _owner->getActorKey();
	}

	void ActorComponent::setOwner_(CommonActor* owner) noexcept
	{
		if (nullptr == owner)
		{
			TA_ASSERT_DEV(false, "널포인터입니다.");
			return;
		}

		_owner = owner;
	}

	bool ActorComponent::getComponentValid_(void) const noexcept
	{
		return _componentValid;
	}

	void ActorComponent::setComponentValid_(const bool value) noexcept
	{
		_componentValid = value;
	}
}

