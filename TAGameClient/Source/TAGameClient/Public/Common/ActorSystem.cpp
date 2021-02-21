#include "Common/ActorSystem.h"


namespace ta
{
	void ActorSystem::update(const ActorSystemUpdateParameter& updateParameter) const noexcept
	{
	}

	const ActorSystemType ActorSystem::getActorSystemType(void) const noexcept
	{
		return _actorSystemType;
	}

	ActorSystem::ActorSystem(const ActorSystemType& actorSystemType) noexcept
		: _actorSystemType(actorSystemType)
	{
	}

	ActorSystem::~ActorSystem(void) noexcept
	{
	}
}


namespace ta
{
	ActorSystemUpdateParameter::ActorSystemUpdateParameter(void) noexcept
	{
		reset();
	}

	ActorSystemUpdateParameter::~ActorSystemUpdateParameter(void) noexcept
	{
	}

	void ActorSystemUpdateParameter::reset(void) noexcept
	{
		_deltaTime = 0;
		_componentCount = 0;
		_components = nullptr;
	}
}