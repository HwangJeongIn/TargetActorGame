#pragma once

#include "Common/ActorSystem.h"


namespace ta
{
	class CommonCharacterActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonCharacterActorSystem(void) noexcept;

		static const ActorSystemType getActorSystemType(void) noexcept;

	protected:
		CommonCharacterActorSystem(void) noexcept;
	};
}
