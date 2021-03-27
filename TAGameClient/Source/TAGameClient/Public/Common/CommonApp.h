#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/GameDataManager.h"
#include <thread>
#include <vector>


namespace ta
{
	class CommonActorManager;
	class CommonActorSystemManager;
	class Iocp;
	class CommonApp;
	class ActorEventTimer;
	class ActorEventObject;
	class ActorKey;
	class GameDataManager;
	class CommonSpawnDataManager;
}


namespace ta
{
	// 각 Level에 정의
	extern CommonApp* g_app;
	extern void WorkerThread(Iocp* iocp) noexcept;

	// Common에 존재
	extern bool RegisterActorEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept;
	extern bool ProcessActorEvent(ActorKey& targetActorKey, const ActorEventObject* actorEvent) noexcept;
}


namespace ta
{
	class CommonApp : public Uncopyable
	{
	public:
		virtual void run(void) noexcept;

		CommonActorManager* getActorManager(void) noexcept;
		CommonActorSystemManager* getActorSystemManager(void) noexcept;
		GameDataManager* getGameDataManager(void) noexcept;

		bool registerActorEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept;

		~CommonApp(void) noexcept;
	protected:
		CommonApp(void) noexcept;

		// 순서 : 가장 큰개념부터 initialize > open 호출 / 다끝나면 가장 작은개념부터 close호출
		virtual bool initialize(void) noexcept;
		virtual bool initializeDetail(void) noexcept;
		virtual bool open(void) noexcept;
		virtual bool beforeRun(void) noexcept;
		virtual void close(void) noexcept;

		void waitAndClose(void) noexcept;

		void sendThreadEndEventToAllThread(void) noexcept;
		void waitForAllThreads(void) noexcept;

	protected:
		Iocp* _iocp;
		ActorEventTimer* _actorEventTimer;
		CommonActorManager* _actorManager;
		CommonActorSystemManager* _actorSystemManager;
		GameDataManager* _gameDataManager;
		CommonSpawnDataManager* _spawnDataManager;


		std::vector<std::thread> _threads;
		const uint32 _maxThreadNum;
	};
}
