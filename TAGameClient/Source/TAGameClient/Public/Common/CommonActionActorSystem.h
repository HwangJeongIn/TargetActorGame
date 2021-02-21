#pragma once

#include "Common/ActorSystem.h"


namespace ta
{
	class CommonCharacterActorComponent;
}


namespace ta
{
	class CommonActionActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonActionActorSystem(void) noexcept;

		static const ActorSystemType getActorSystemType(void) noexcept;

		// process붙은 함수류는 서버에서 돌리는 Ai와 유저가 돌리는 Ai를 둘다 돌리기 위해서 만들었다.
		virtual bool processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept;

	protected:
		CommonActionActorSystem(void) noexcept;

		bool calcAndChangeHp_(const uint32 strength, CommonCharacterActorComponent* targetCom) const noexcept;
		bool changeHp_(const uint32 hpValue, CommonCharacterActorComponent* targetCom) const noexcept;

	};
}
