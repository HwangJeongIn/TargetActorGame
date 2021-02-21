#pragma once

#include "Common/ActorComponent.h"


namespace ta
{
	class CommonCharacterActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonCharacterActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;
		// 각레벨에서 더 필요하다면 더 내려갈 수 있다.
		virtual ComponentData* packData_(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;
	protected:
		CommonCharacterActorComponent(void) noexcept;

	public:
		uint32 _strength;
		uint32 _agility;
		
		uint32 _maxHp;
		uint32 _currentHp;
	};
}
