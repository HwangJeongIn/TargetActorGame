#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonActorSystemManager.h"


namespace ta
{
	class CommonActorManager;
}


namespace ta
{
	class ClientActorSystemManager : public CommonActorSystemManager
	{
		friend class ClientApp; // 여기서만 만들어야해서 프렌드 선언
	public:
		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

	private:

		explicit ClientActorSystemManager(CommonActorManager* actorManager) noexcept;
		virtual ~ClientActorSystemManager(void) noexcept;

	};

}
