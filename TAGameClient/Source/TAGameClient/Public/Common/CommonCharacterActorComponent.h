#pragma once

#include "Common/ActorComponent.h"


namespace ta
{
	class CharacterGameData;
	class RenderingGameData;
}


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

		const CharacterGameData* getCharacterGameData_(void) const noexcept;
		const RenderingGameData* getRenderingGameData_(void) const noexcept;

		float getCurrentHp_(void) const noexcept;
		bool setCurrentHp_(const float value) noexcept;

	protected:
		CommonCharacterActorComponent(void) noexcept;

	private:
		const CharacterGameData* _characterGameData;
		float _currentHp;
	};
}
