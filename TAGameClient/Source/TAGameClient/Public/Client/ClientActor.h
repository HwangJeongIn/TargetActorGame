#pragma once

#ifndef TA_SERVER
#include "TACharacter.h"
#endif

#include "Common/CommonActor.h"


namespace ta
{
	class ClientActor : public CommonActor
	{
		friend class ClientActorDataPool; // 풀에 만들때만 사용해야해서 프렌드 선언
	public:
#ifndef TA_SERVER
		ATACharacter* getUnrealCharacter_(void) noexcept;
		bool setUnrealCharacter_(ATACharacter* character) noexcept;
		bool resetUnrealCharacter_(void) noexcept;
#endif
	
	private:
		ClientActor(void) noexcept;
		virtual ~ClientActor(void) noexcept;

		virtual void onToPool(void) noexcept override final;
		virtual void onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept override final;
		virtual void onActive(void) noexcept override final;

	private:
#ifndef TA_SERVER
		//TSharedPtr<ATACharacter> _unrealCharacter;
		ATACharacter* _unrealCharacter;
#endif
	};
}
