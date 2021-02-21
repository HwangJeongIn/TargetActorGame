#pragma once

#include "Common/CommonCharacterActorComponent.h"


namespace ta
{
	class ClientCharacterActorComponent : public CommonCharacterActorComponent
	{
	public:
		ClientCharacterActorComponent(void) noexcept;
		virtual ~ClientCharacterActorComponent(void) noexcept;
	private:
	};
}
