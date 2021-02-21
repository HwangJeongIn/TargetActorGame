#pragma once

#include "Common/CommonMoveActorComponent.h"


namespace ta
{
	class ClientMoveActorComponent : public CommonMoveActorComponent
	{
	public:
		ClientMoveActorComponent(void) noexcept;
		virtual ~ClientMoveActorComponent(void) noexcept;
		virtual void onActive(void) noexcept override final;
	private:
	};
}
