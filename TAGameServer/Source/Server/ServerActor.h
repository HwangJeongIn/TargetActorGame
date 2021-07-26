#pragma once

#include "Common/CommonActor.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	class ServerActor : public CommonActor
	{
		friend class ServerActorDataPool; // 풀에 만들때만 사용해야해서 프렌드 선언
	public:
		const CharacterDBNoKey& getCharacterDBNoKey_(void) const noexcept;
		void setCharacterDBNoKey_(const CharacterDBNoKey& characterDBNoKey) noexcept;

	private:
		ServerActor(void) noexcept;
		virtual ~ServerActor(void) noexcept;

		virtual void onToPool(void) noexcept override final;
		virtual void onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept override final;



	private:
		CharacterDBNoKey _characterDBNoKey; // DB에서 불러온값 // 플레이어 관련 액터가 아닌경우 -1이다.
	};
}
