#pragma once

#include "Common/ActorComponent.h"
#include <unordered_map>


namespace ta
{
	class CharacterGameData;
	class BuffObject;
	class ContentParameter;
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

		bool setCharacterGameData_(const CharacterGameDataKey& characterGameDataKey) noexcept;
		const CharacterGameData* getCharacterGameData_(void) const noexcept;
		const RenderingGameData* getRenderingGameData_(void) const noexcept;

		float getCurrentStat_(const CharacterStatType statType) const noexcept;
		bool setCurrentStat_(const CharacterStatType statType, const float value) noexcept;
		bool addCurrentStat_(const CharacterStatType statType, const float value) noexcept;

		float getCurrentHp_(void) const noexcept;
		bool setCurrentHp_(const float value) noexcept;

		BuffObject* getBuff_(const BuffGameDataKey& buffGameDataKey) noexcept;
		bool registerBuff_(const BuffGameDataKey& buffGameDataKey, BuffObject* buffObject) noexcept;
		bool unregisterBuff_(const BuffGameDataKey& buffGameDataKey) noexcept;

	protected:
		CommonCharacterActorComponent(void) noexcept;

	private:
		void onChangedCharacterGameData(void) noexcept;


	private:
		const CharacterGameData* _characterGameData;

		float _currentStats[static_cast<uint8>(CharacterStatType::Count)];
		std::unordered_map<BuffGameDataKey, BuffObject*> _currentBuffSet;
	};
}
