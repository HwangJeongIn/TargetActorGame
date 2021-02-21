#pragma once

#include "Common/CommonBase.h"
#include "Common/CommonApp.h"


namespace ta
{
	class ServerActorManager;
	class ServerActorSystemManager;
	class ActorKey;
	class ActorEventObject;
}

// 다른곳에서 사용할떄 이 해더만 인클루드하면 사용할 수 있음
// 원래 싱글톤으로 가려했으나 다른곳에서 변수를 선언하지않을거라 호출 오버해드가 적은 전역변수를 선택했다.
namespace ta
{
	extern CommonApp* g_app;
	extern ServerActorManager* GetActorManager(void) noexcept;
	extern ServerActorSystemManager* GetActorSystemManager(void) noexcept;
	extern void WorkerThread(Iocp* iocp) noexcept;
	extern void SendAllComponentDataToClient(const ActorKey& targetActorKey, const ActorKey& actorKeyToSend) noexcept;
}


namespace ta
{
	class ServerApp : public CommonApp
	{
	public:
		ServerApp(void) noexcept;
		~ServerApp(void) noexcept;

		virtual void run(void) noexcept override final;

	private:
		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual bool beforeRun(void) noexcept override final;
		virtual void close(void) noexcept override final;

	private:

		void logTest(void) noexcept;
	};
}