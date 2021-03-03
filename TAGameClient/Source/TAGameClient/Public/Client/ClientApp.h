#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonApp.h"
#include <thread>
#include <vector>


namespace ta
{
	class ClientActorManager;
	class ClientActorSystemManager;
	class ClientActor;
	class ActorKey;
	class Iocp;
}

// 다른곳에서 사용할떄 이 해더만 인클루드하면 사용할 수 있음
// 원래 싱글톤으로 가려했으나 다른곳에서 변수를 선언하지않을거라 호출 오버해드가 적은 전역변수를 선택했다.
namespace ta
{
	extern CommonApp* g_app;
	extern ClientActorManager* GetActorManager(void) noexcept;
	extern ClientActorSystemManager* GetActorSystemManager(void) noexcept;
	extern ClientActor* GetMainPlayerActor(void) noexcept;
	extern void WorkerThread(Iocp* iocp) noexcept;
}


namespace ta
{
	class ClientApp : public CommonApp
	{
	public:
		ClientApp(void) noexcept;
		~ClientApp(void) noexcept;

		virtual void run(void) noexcept override final;

		void finishConnect(const ActorKey& createdActorFromServer) noexcept;

	private:
		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

	};
}



