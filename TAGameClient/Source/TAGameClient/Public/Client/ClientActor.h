#pragma once

#ifndef TA_SERVER
class AActor;
#endif

#include "Common/CommonActor.h"


namespace ta
{
	class ClientActor : public CommonActor
	{
		friend class ClientActorDataPool; // 풀에 만들때만 사용해야해서 프렌드 선언
	public:
#ifndef TA_SERVER
		AActor* getUnrealActor_(void) noexcept;
		bool setUnrealActor_(AActor* character) noexcept;
		bool resetUnrealActor_(void) noexcept;
#endif
	
	private:
		ClientActor(void) noexcept;
		virtual ~ClientActor(void) noexcept;

		virtual void onToPool(void) noexcept override final;
		virtual void onFromPool(const CommonActorBasicSpawnData& spawnData) noexcept override final;
		virtual void onActive(void) noexcept override final;

	private:
#ifndef TA_SERVER
		//TSharedPtr<ATACharacter> _unrealActor;
		AActor* _unrealActor;
#endif
	};
}
