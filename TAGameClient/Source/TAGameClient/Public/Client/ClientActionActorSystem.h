#pragma once

#include "Common/CommonActionActorSystem.h"


namespace ta
{
	class ActorKey;
}


namespace ta
{
	class ClientActionActorSystem : public CommonActionActorSystem
	{
	public:
		ClientActionActorSystem(void) noexcept;
		virtual ~ClientActionActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept override final;

		virtual bool processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept override final;

		bool requestAttackTarget(const ActorKey& attacker, const ActorKey& targetActorKey) const noexcept;

		bool respondChangeHp(const ActorKey& attacker
							 , const ActorKey& targetActorKey
							 , const float& hpValue) const noexcept;

		bool respondKillActor(const ActorKey& targetActorKey) const noexcept;
	};
}
