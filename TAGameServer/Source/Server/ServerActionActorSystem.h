#pragma once

#include "Common/CommonActionActorSystem.h"


namespace ta
{
	class ActorKey;
}


namespace ta
{
	class ServerActionActorSystem : public CommonActionActorSystem
	{
	public:
		ServerActionActorSystem(void) noexcept;
		virtual ~ServerActionActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;

		virtual bool processAttackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept override final;

		bool attackTarget(CommonCharacterActorComponent* myCom, CommonCharacterActorComponent* targetCom) const noexcept;
		bool respondAttackTarget(const ActorKey& myActorKey, const ActorKey& targetActorKey) const noexcept;
	private:

	};
}
