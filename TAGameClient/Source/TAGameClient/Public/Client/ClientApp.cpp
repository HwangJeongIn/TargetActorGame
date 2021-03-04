#include "Client/ClientApp.h"
#include "Client/ClientActorManager.h"
#include "Client/ClientActorSystemManager.h"
#include "Client/AllPacketClient.h"
#include "Client/ClientSpawnDataManager.h"
#include "Client/ClientActor.h"
#include "Common/Iocp.h"
#include "Common/Socket.h"
#include "Common/CommonActor.h"
#include "Common/EndPoint.h"
#include "Common/CommonActorManager.h"
#include "Common/AllPacketCommon.h"
#include "Common/ActorEventTimer.h"
#include "Common/ActorEvent.h"
#include <thread>




namespace ta
{
	CommonApp* g_app = new ClientApp;

	ClientActorManager* GetActorManager(void) noexcept
	{
		return static_cast<ClientActorManager*>(g_app->getActorManager());
	}

	ClientActorSystemManager* GetActorSystemManager(void) noexcept
	{
		return static_cast<ClientActorSystemManager*>(g_app->getActorSystemManager());
	}

	ClientActor* GetMainPlayerActor(void) noexcept
	{
		return static_cast<ClientActor*>(GetActorManager()->getMainPlayerActor());
	}

	void WorkerThread(Iocp* iocp) noexcept
	{
		DWORD numberOfBytesTransferred = 0;
		ULONG_PTR completionKey = 0;
		WSAOVERLAPPED* overlapped = nullptr;

		while (true)
		{
			numberOfBytesTransferred = 0;
			completionKey = 0;
			overlapped = nullptr;

			GetQueuedCompletionStatus(iocp->_handle // 소켓을 등록한 IOCP
									  , &numberOfBytesTransferred // 전송된 데이터의 양, overlapped I/O 콜백에도 해당 파라미터가 들어온다
									  , &completionKey // 아까 소켓별로 등록해둔 Key
									  , &overlapped // overlapped I/O 구조체
									  , INFINITE); // time out

			OverlappedStructBase* overlappedStructBase = reinterpret_cast<OverlappedStructBase*>(overlapped);
			const ThreadWorkType workType = overlappedStructBase->_workType;
			// Connect / Accept는 모두 ActorKey가 0인 액터가 사용된다.
			// Connect의 경우 클라레벨에서 ClientOwnerActor이고
			// Accept의 경우 서버레벨에서 ListenSocket이다.

			ActorKey targetActorKey;
			Socket* network = nullptr;
			if (workType < ThreadWorkType::SocketEventEnd)
			{
				targetActorKey = (GetActorManager()->getMainActorKeyWithCompletionKey(ActorKey(completionKey)));
				CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
				TA_ASSERT_DEV((nullptr != targetActor), "제대로 초기화 안됐습니다.");
				network = targetActor->getNetwork_();
				TA_ASSERT_DEV((nullptr != network), "제대로 초기화 안됐습니다.");
			}
			else
			{
				targetActorKey = ActorKey(completionKey);
			}

			switch (workType)
			{
			case ThreadWorkType::SocketConnect:
				{
					// 클라레벨에서만 클라오너가 들어온다.
					network->updateConnectContext();
					network->receiveOverlapped();

					// 원래라면 UI를 통해서 해줘야하지만 일단 바로 로그인 시킨다.
					// ID / Password 같이 넘겨주자
					TA_LOG_DEV("client is connected / send login packet");
					SendLoginCTS(targetActorKey, StringPack("MyId"), StringPack("MyPassword"));
				}
				break;
			case ThreadWorkType::SocketReceive:
				{
					// 클라레벨 서버레벨 공통
					if (0 == numberOfBytesTransferred)
					{
						// ? 무슨상황인지 파악하자
						// 일단 disconnect

					}
					else // 잘려왔거나 그냥왔거나 재조립해야할 수도 있다.
					{
						ProcessReceive(targetActorKey, numberOfBytesTransferred);
					}

					network->receiveOverlapped();
				}
				break;
			case ThreadWorkType::SocketSend:
				{
					// 클라레벨 서버레벨 공통

					// 임시로 동적할당해서 보냈기 때문에 삭제해줘야 한다.
					if (0 == numberOfBytesTransferred)
					{
						// disconnect
					}

					delete overlappedStructBase;
				}
				break;
			case ThreadWorkType::ActorEvent:
				{
					ActorEventOverlappedStruct* actorEventOverlappedStruct = static_cast<ActorEventOverlappedStruct*>(overlappedStructBase);
					ProcessActorEvent(targetActorKey, actorEventOverlappedStruct->_actorEventObject);

					delete actorEventOverlappedStruct;
				}
				break;
			case ThreadWorkType::ThreadEnd:
				{
					delete overlappedStructBase;
					return;
				}
				break;
			}
		}
	}
}


namespace ta
{
	ClientApp::ClientApp(void) noexcept
	{

	}

	ClientApp::~ClientApp(void) noexcept
	{

	}

	void ClientApp::run(void) noexcept
	{
		if (false == initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return;
		}

		if (false == open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return;
		}

		if (false == beforeRun())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return;
		}

		CommonApp::run();

		Socket* network = _actorManager->getOwnerActor()->getNetwork_();
		network->bind(Endpoint());

		_iocp->bindSocket(*network, _actorManager->getOwnerActor()->getActorKey());
		network->connectOverlapped(Endpoint("127.0.0.1", 5555));

#ifdef TA_SERVER
		waitAndClose();
#endif
	}

	void ClientApp::finishConnect(const ActorKey& createdActorFromServer) noexcept
	{
		// 컴플리션키 변경 안된다. 그냥 메인액터랑 0번(Client Owner)이랑 연결만하고 컴플리션 포트에서 처리하는 곳에서 메인액터키 얻을 수 있게 해주자
		//_iocp->bindSocket(*(_actorManager->getOwnerActor()->getNetwork_()), createdActorFromServer);
		
		// 해당 키에 대해서 액터 생성
		ClientActorManager* clinetActorManager = static_cast<ClientActorManager*>(_actorManager);
		clinetActorManager->createActorForClient(ActorType::Player, createdActorFromServer);
		clinetActorManager->bindActorAsMainPlayerActor(createdActorFromServer);
	}

	bool ClientApp::initialize(void) noexcept
	{
		if (false == CommonApp::initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		_actorManager = new ClientActorManager;
		_actorSystemManager = new ClientActorSystemManager(_actorManager);
		_spawnDataManager = new ClientSpawnDataManager;

		if (false == initializeDetail())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	bool ClientApp::open(void) noexcept
	{
		if (false == CommonApp::open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	void ClientApp::close(void) noexcept
	{
		CommonApp::close();
	}
}

