#pragma once

#include "Common/CommonCharacterActorSystem.h"


namespace ta
{
	class CommonCharacterActorComponent;
	class BuffGameDataKey;
	class ContentParameter;
}


namespace ta
{
	class ServerCharacterActorSystem : public CommonCharacterActorSystem
	{
	public:
		ServerCharacterActorSystem(void) noexcept;
		virtual ~ServerCharacterActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept override final;

		virtual bool doBuff(CommonCharacterActorComponent* myCharacter
							, const BuffGameDataKey& buffGameDataKey
							, const ContentParameter& parameter) noexcept override final;

		virtual bool undoBuff(CommonCharacterActorComponent* myCharacter
							  , const BuffGameDataKey& buffGameDataKey
							  , const ContentParameter& parameter) noexcept override final;
	};
}
