#include "Common/CommonApp.h"
#include "Common/Iocp.h"
#include "Common/Socket.h"
#include "Common/CommonActorManager.h"
#include "Common/CommonActorSystemManager.h"
#include "Common/GameDataManager.h"
#include "Common/CommonSpawnDataManager.h"
#include "Common/ActorEventTimer.h"
#include "Common/CommonActor.h"
#include "Common/CommonAiActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ActorEvent.h"
#include "Common/AiBehaviorTree.h"



namespace ta
{
	bool RegisterActorEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept
	{
		return g_app->registerActorEvent(actorEventObject, delayMilliSec);
	}


	bool ProcessActorEvent(ActorKey& targetActorKey, const ActorEventObject* actorEvent) noexcept
	{
		CommonActor* targetActor = g_app->getActorManager()->getActor(targetActorKey, true);
		bool rv = true;
		TA_ASSERT_DEV((nullptr != targetActor), "비정상입니다.");

		switch (actorEvent->_actorEventType)
		{
		case ActorEventType::AiEvent:
			{
				if (ActorType::Player == targetActor->getActorType())
				{
					break;
				}

				CommonAiActorSystem* aiActorSystem = GetActorSystem<CommonAiActorSystem>();

				switch (actorEvent->_aiCommandType)
				{
				case AiCommandType::TickAi:
					{
						rv = aiActorSystem->processCommandTickAi(targetActor, actorEvent->_deltaTime);
					}
					break;
				case AiCommandType::Move:
					{

					}
					break;
				case AiCommandType::Attack:
					{

					}
					break;
				default:
					break;
				}

				TA_COMPILE_DEV(3 == static_cast<uint8>(AiCommandType::Count), "여기도 추가해주세요")
			}
			break;
		case ActorEventType::LogTest:
			{
				g_app->getActorManager()->logTest();

				ActorEventObject* logEvent = new ActorEventObject;
				logEvent->_actorEventType = ActorEventType::LogTest;
				logEvent->_myActorKey = targetActorKey;

				if (false == RegisterActorEvent(logEvent, 3000))
				{
					TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
				}
			}
			break;
			// 앞에서 처리된다.
		//case ActorEventType::ThreadEnd:
		//	{
		//		
		//	}
		//	break;
		default:
			break;
		}

		TA_COMPILE_DEV(3 == static_cast<uint8>(ActorEventType::Count), "여기도 추가해주세요");
			return rv;
	}
}


namespace ta
{
	CommonApp::CommonApp(void) noexcept
		: _maxThreadNum(10)
	{
	}

	CommonApp::~CommonApp(void) noexcept
	{
	}

	void CommonApp::run(void) noexcept
	{
		_threads.reserve(_maxThreadNum);
		_threads.emplace_back(ActorEventTimerThread, _actorEventTimer);

		for (uint32 threadIndex = _threads.size(); threadIndex < _maxThreadNum; ++threadIndex)
		{
			_threads.emplace_back(WorkerThread, _iocp);
		}
	}

	CommonActorManager* CommonApp::getActorManager(void) noexcept
	{
		return _actorManager;
	}

	CommonActorSystemManager* CommonApp::getActorSystemManager(void) noexcept
	{
		return _actorSystemManager;
	}

	GameDataManager* CommonApp::getGameDataManager(void) noexcept
	{
		return _gameDataManager;
	}

	bool CommonApp::registerActorEvent(ActorEventObject* actorEventObject, const long long delayMilliSec) noexcept
	{
		return _actorEventTimer->addEvent(actorEventObject, delayMilliSec);
	}

	bool CommonApp::initialize(void) noexcept
	{
		_iocp = new Iocp;
		_gameDataManager = new GameDataManager();
		_actorEventTimer = new ActorEventTimer(_iocp);
		_actorManager = nullptr;
		_actorSystemManager = nullptr;
		_spawnDataManager = nullptr;

		return true;
	}

	bool CommonApp::initializeDetail(void) noexcept
	{
		if (false == _gameDataManager->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _spawnDataManager->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorEventTimer->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorManager->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorSystemManager->initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		AiBehaviorTree::initializeAllAi();

		return true;
	}

	bool CommonApp::open(void) noexcept
	{
		Socket::Startup();

		if (false == _gameDataManager->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _spawnDataManager->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorEventTimer->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorManager->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		if (false == _actorSystemManager->open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	bool CommonApp::beforeRun(void) noexcept
	{
		return true;
	}

	void CommonApp::close(void) noexcept
	{
		Socket::Cleanup();

		_actorSystemManager->close();
		delete _actorSystemManager;

		_actorManager->close();
		delete _actorManager;

		_actorEventTimer->close();
		delete _actorEventTimer;

		_gameDataManager->close();
		delete _gameDataManager;

		_spawnDataManager->close();
		delete _spawnDataManager;

		delete _iocp;
	}

	void CommonApp::waitAndClose(void) noexcept
	{
		waitForAllThreads();
		close();
	}

	void CommonApp::sendThreadEndEventToAllThread(void) noexcept
	{
		// 이벤트 타이머 스레드한테도 보내고
		ActorEventObject* actorEventObject = new ActorEventObject;
		actorEventObject->_actorEventType = ActorEventType::ThreadEnd;
		registerActorEvent(actorEventObject, 0);

		// 나머지 스레드한테도 보낸다.
		uint32 workerCount = _threads.size() -1;
		for (uint32 index = 0; index < workerCount; ++index)
		{
			OverlappedStructBase* overlappedStruct = new OverlappedStructBase;
			overlappedStruct->_workType = ThreadWorkType::ActorEvent;

			PostQueuedCompletionStatus(_iocp->_handle, 1, 0, &overlappedStruct->_wsaOverlapped);
		}
	}

	void CommonApp::waitForAllThreads(void) noexcept
	{
		for (uint32 threadIndex = 0; threadIndex < _maxThreadNum; ++threadIndex)
		{
			_threads[threadIndex].join();
		}
	}
}