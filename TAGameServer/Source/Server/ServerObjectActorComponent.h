#pragma once

#include "Common/CommonObjectActorComponent.h"


namespace ta
{
	class ServerObjectActorComponent : public CommonObjectActorComponent
	{
	public:
		ServerObjectActorComponent(void) noexcept;
		virtual ~ServerObjectActorComponent(void) noexcept;
	};
}
