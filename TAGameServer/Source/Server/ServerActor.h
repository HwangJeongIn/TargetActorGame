#pragma once

#include "Common/CommonActor.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	class ServerActor : public CommonActor
	{
		friend class ServerActorDataPool; // 풀에 만들때만 사용해야해서 프렌드 선언
	public:
		const CharacterDBNo& getCharacterDBNo_(void) const noexcept;
		void setCharacterDBNo_(const CharacterDBNo& characterDBNo) noexcept;

	private:
		ServerActor(void) noexcept;
		virtual ~ServerActor(void) noexcept;

		virtual void onToPool(void) noexcept override final;
		virtual void onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept override final;



	private:
		CharacterDBNo _characterDBNo; // DB에서 불러온값 // 플레이어 관련 액터가 아닌경우 -1이다.
	};
}
