#pragma once

#include "Common/CommonCharacterActorSystem.h"


namespace ta
{
	class ClientCharacterActorSystem : public CommonCharacterActorSystem
	{
	public:
		ClientCharacterActorSystem(void) noexcept;
		virtual ~ClientCharacterActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept override final;

	};
}
