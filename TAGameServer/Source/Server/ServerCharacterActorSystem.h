#pragma once

#include "Common/CommonCharacterActorSystem.h"


namespace ta
{
	class ServerCharacterActorSystem : public CommonCharacterActorSystem
	{
	public:
		ServerCharacterActorSystem(void) noexcept;
		virtual ~ServerCharacterActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept override final;

	};
}
