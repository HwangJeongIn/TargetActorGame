#pragma once

#include "Common/ActorSystem.h"


namespace ta
{
	class CommonCharacterActorComponent;
	class BuffGameDataKey;
	class ContentParameter;
	class BuffObject;
}


namespace ta
{
	class CommonCharacterActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonCharacterActorSystem(void) noexcept;

		static const ActorSystemType getActorSystemType(void) noexcept;

		BuffObject* checkAndRegisterBuff(CommonCharacterActorComponent* myCharacter
										 , const BuffGameDataKey& buffGameDataKey
										 , const ContentParameter& parameter) noexcept;

		BuffObject* checkAndUnregisterBuff(CommonCharacterActorComponent* myCharacter
										   , const BuffGameDataKey& buffGameDataKey
										   , const ContentParameter& parameter) noexcept;

		virtual bool doBuff(CommonCharacterActorComponent* myCharacter
							, const BuffGameDataKey& buffGameDataKey
							, const ContentParameter& parameter) noexcept;

		virtual bool undoBuff(CommonCharacterActorComponent* myCharacter
							  , const BuffGameDataKey& buffGameDataKey
							  , const ContentParameter& parameter) noexcept;

		bool addStat(CommonCharacterActorComponent* myCharacter, const CharacterStatType statType, const float point);

	protected:
		CommonCharacterActorSystem(void) noexcept;
	};
}
