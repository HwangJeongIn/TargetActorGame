#pragma once

#include "Common/ActorComponent.h"


namespace ta
{
	class CommonActionActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonActionActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;
		// 각레벨에서 더 필요하다면 더 내려갈 수 있다.
		virtual ComponentData* packData_(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;
	protected:
		CommonActionActorComponent(void) noexcept;
		
	public:
	};
}
