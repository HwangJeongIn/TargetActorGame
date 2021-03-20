#include "Server/ServerApp.h"
#include "Server/ServerActorManager.h"
#include "Server/ServerActorSystemManager.h"
#include "Server/AllPacketServer.h"
#include "Server/ServerSpawnDataManager.h"
#include "Common/Iocp.h"
#include "Common/Socket.h"
#include "Common/CommonActor.h"
#include "Common/EndPoint.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"
#include "Common/ActorEventTimer.h"
#include "Common/ActorComponentTypeList.h"
#include "Common/ActorEvent.h"
#include <thread>
//test
#include "Common/Serializer.h"
#include "Common/FileLoader.h"



namespace ta
{
	CommonApp* g_app = new ServerApp;

	ServerActorManager* GetActorManager(void) noexcept
	{
		return static_cast<ServerActorManager*>(g_app->getActorManager());
	}

	ServerActorSystemManager* getActorSystemManager(void) noexcept
	{
		return static_cast<ServerActorSystemManager*>(g_app->getActorSystemManager());
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
				targetActorKey = ActorKey(completionKey);
				CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
				TA_ASSERT_DEV((nullptr != targetActor), "제대로 초기화 안됐습니다.");
				network = targetActor->getNetwork_();
				TA_ASSERT_DEV((nullptr != network), "제대로 초기화 안됐습니다.");
			}
			else
			{
				targetActorKey = ActorKey(completionKey);
			}

			// Connect / Accept는 모두 ActorKey가 0인 액터가 사용된다.
			// Connect의 경우 클라레벨에서 ClientOwnerActor이고
			// Accept의 경우 서버레벨에서 ListenSocket이다.

			switch (overlappedStructBase->_workType)
			{
			case ThreadWorkType::SocketAccept:
				{
					CustomOverlappedStruct* customOverlappedStruct = static_cast<CustomOverlappedStruct*>(overlappedStructBase);
					CommonActor* newClient = GetActorManager()->createActorForServer(ActorType::Player);
					if (nullptr == newClient)
					{
						TA_ASSERT_DEV(false, "비정상입니다.");
						return;
					}

					//ScopedLock lock(newClient); // 여기서도 다른스레드에서 이 클라이언트를 바꾸지 않을것이기 때문에 괜찮다.
					//현재 state가 init이라서 다른곳에서 사용할 수 없다.
					Socket* newClientNetwork = newClient->getNetwork_();
					TA_ASSERT_DEV((nullptr != newClientNetwork), "코드버그");
					// 기본으로 만들어진 소켓에 현재 연결된 소켓으로 변경
					newClientNetwork->setSocketHandle(customOverlappedStruct->_acceptedClientSocket);
					// 최종적으로 Accept마무리
					newClientNetwork->updateAcceptContext(*network);

					// 클라레벨에서 어디서 들어왔는지?
					Endpoint peerAddr;
					newClientNetwork->getPeerAddr(peerAddr);
					TA_LOG_DEV("accept client : %s", peerAddr.toTstring().data());

					// IOCP등록후 Receive대기
					iocp->bindSocket(*newClientNetwork, newClient->getActorKey());
					// 새로운 클라이언트 recv대기
					// 아이디 비밀번호를 담은 로그인 패킷이 오면 DB에서 받아서 업데이트하고 최종적으로 Active상태가 된다.
					newClientNetwork->receiveOverlapped();

					// 리슨소켓 accept대기
					network->acceptOverlapped();

					// 해당 클라에게 몇번 액터 쓰라고 알려준다.
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
	void SendAllComponentDataToClient(const ActorKey& targetActorKey, const ActorKey& actorKeyToSend) noexcept
	{
		CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
		if (nullptr == targetActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
				return;
		}

		const ActorType targetActorType = targetActor->getActorType();
		if (ActorType::Player != targetActorType)
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
				return;
		}

		CommonActor* actorToSend = GetActorManager()->getActor(actorKeyToSend);
		if (nullptr == actorToSend)
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
				return;
		}


		ComponentData* componentData = nullptr;
		ActorComponent* actorComponent = nullptr;
		ActorComponentType currentType = ActorComponentType::Count;
		const std::vector<ActorComponentType>& componentTypeList = ActorComponentGroups.at(actorToSend->getActorType());
		const uint32 count = componentTypeList.size();
		for (uint32 index = 0; index < count; ++index)
		{
			actorComponent = GetActorComponent(actorKeyToSend, componentTypeList[index]);
			{
				ScopedLock componentLock(actorComponent);

				if (false == actorComponent->getComponentValid_())
				{
					continue;
				}

				componentData = actorComponent->packData_();
			}

			switch (componentTypeList[index])
			{
#define COMPONENTDATA_SEND(Type)																											  \
																																			  \
			case ActorComponentType::Type:																									  \
				{																															  \
					Send##Type##ComponentDataSTC(targetActorKey, actorKeyToSend, static_cast<Common##Type##ComponentData&>(*componentData));  \
				}																															  \
				break;

				COMPONENTDATA_SEND(Move)
				COMPONENTDATA_SEND(Action)
				COMPONENTDATA_SEND(Ai)
				COMPONENTDATA_SEND(Character)
				COMPONENTDATA_SEND(Inventory)

			default:
					{
						TA_COMPILE_DEV((5 == static_cast<uint8>(ActorComponentType::Count)), "여기도 추가해주세요")
					}
					break;
			}

			delete componentData;
			componentData = nullptr;
		}
	}
}


namespace ta
{
	ServerApp::ServerApp(void) noexcept
	{

	}

	ServerApp::~ServerApp(void) noexcept
	{

	}

	void ServerApp::run(void) noexcept
	{
		// 테스트

		if(false)
		{
			std::string filePath("E:/GitLocal/TargetActorGame/TAGameServer/test.txt");

			Serializer slW;
			Serializer slR;

			{
				//sl.allocBuffer(1024 * 1024);
				slW.setModeFlag(SerializerMode::Write);
				int8 a = 1;
				int16 b = 2;
				int32 c = 3;

				slW << a << b << c;
				slW.exportToFile(filePath);
				//FileLoader::saveFileString(filePath, fileString);
			}

			int8 outA = 0;
			int16 outB = 0;
			int32 outC = 0;
			{
				//sl.allocBuffer(1024 * 1024);
				slR.setModeFlag(SerializerMode::Read);
				slR.importFromFile(filePath);
				slR << outA << outB << outC;
			}
		}



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


		//logTest();

		Socket* network = _actorManager->getOwnerActor()->getNetwork_();
		network->bind(Endpoint("0.0.0.0", 5555));
		network->listen();

		_iocp->bindSocket(*network, _actorManager->getOwnerActor()->getActorKey());
		network->acceptOverlapped();

		waitAndClose();
	}

	bool ServerApp::initialize(void) noexcept
	{
		if (false == CommonApp::initialize())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		_actorManager = new ServerActorManager;
		_actorSystemManager = new ServerActorSystemManager(_actorManager);
		_spawnDataManager = new ServerSpawnDataManager;

		if (false == initializeDetail())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	bool ServerApp::open(void) noexcept
	{
		if (false == CommonApp::open())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	bool ServerApp::beforeRun(void) noexcept
	{
		if (false == CommonApp::beforeRun())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		//스폰 데이터로 스폰
		if (false == static_cast<ServerSpawnDataManager*>(_spawnDataManager)->spawnRealWorldActors())
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		return true;
	}

	void ServerApp::close(void) noexcept
	{
		CommonApp::close();
	}
	
	void ServerApp::logTest(void) noexcept
	{
		ActorEventObject* logEvent = new ActorEventObject;
		logEvent->_actorEventType = ActorEventType::LogTest;
		logEvent->_myActorKey = 1;
		if (false == RegisterActorEvent(logEvent, 3000))
		{
			TA_ASSERT_DEV(false, "이벤트 등록에 실패했습니다.");
		}
	}
}