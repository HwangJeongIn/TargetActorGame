#pragma once

#include "Common/CommonCharacterActorComponent.h"


namespace ta
{
	class ServerCharacterActorComponent : public CommonCharacterActorComponent
	{
	public:
		ServerCharacterActorComponent(void) noexcept;
		virtual ~ServerCharacterActorComponent(void) noexcept;

	private:
	};
}
