
#include "Common/Socket.h"
#include "Common/Endpoint.h"
#include "Common/Iocp.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/CommonActor.h"
#include "Common/ScopedLock.h"
#include "Common/MessagePack.h"
#include "Common/AllPacketCommon.h"
#include "Common/ActorDataGroups.h"


#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif


namespace ta
{
	bool ProcessReceive(const ActorKey& targetActorKey, const DWORD numberOfBytesTransferred) noexcept
	{
		CommonActor* targetActor = g_app->getActorManager()->getActor(targetActorKey);
		if (nullptr == targetActor)
		{
			TA_ASSERT_DEV(false, "액터가 없습니다.");
			return false;
		}

		// 일단 받은 버퍼를 가지고온다
		Socket* network = targetActor->getNetwork_();
		if (nullptr == network)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}


		/*
		1. 일단 패킷사이즈 판단하는게 우선 // 첫 4바이트 (uint32)를 읽어서 확인하자
		   받아놓은 버퍼가 없다면 지금 막들어온 버퍼의 첫 4바이트를 읽고, 받아둔 버퍼가 있다면 받아놓은 버퍼가 앞조각이다.

		2. 그 패킷사이즈 기준으로 버퍼를 붙여서 완성한다.
			2.1. 받아놓은 버퍼가 있을 때
			 : prevRecvSize 위치부터 들어온 버퍼를 카피한다. 이때 패킷사이즈를 넘지 않도록한다.
			   여기서 패킷사이즈만큼 만들어 졌다면, 패킷을 처리 + 남은 바이트 있으면 처리 부족하다면 카피하고 끝낸다.

			2.2. 받아놓은 버퍼가 없을 때
			 : 들어온 바이트가 패킷사이즈보다 크거나 같으면 패킷처리후 남은 바이트 카피
			   만약에 더 적으면 카피만 하고 끝난다.
		*/

		char* inputBuffer = network->_readOverlappedStruct._iocpBuf;
		int restDataSize = numberOfBytesTransferred;
		uint32 packetSize = 0;
		bool rv = true; // 패킷처리 됐는지 여부
		while (0 < restDataSize)
		{
			if (0 == network->_prevRecvSize) // 받아둔 패킷이 없는경우
			{
				packetSize = reinterpret_cast<MessagePack*>(inputBuffer)->_length;
			}
			else // 받아둔 패킷이 있는경우
			{
				packetSize = reinterpret_cast<MessagePack*>(network->_recvBuffer)->_length;
			}

			if ((network->_prevRecvSize + restDataSize) < packetSize)
			{
				// 부족한 경우
				memcpy(network->_recvBuffer + network->_prevRecvSize, inputBuffer, restDataSize);
				network->_prevRecvSize += restDataSize;
				rv = false;
				break;
			}

			// 같거나 큰경우
			const uint32 rest = packetSize - network->_prevRecvSize;
			memcpy(network->_recvBuffer + network->_prevRecvSize, inputBuffer, rest);
			inputBuffer += rest;
			restDataSize -= rest; // 남아있을 수 있다.

			// 메시지 처리
			// MessageTypeSwtichCaseSet
			// 원래는 객체를 가지고 오고나서 가상함수를 호출했었다.
			// 하지만 다른프로세스에서 받아온 해당 가상함수 포인터 주소가 읽기 위반이 나서(코드영역에 로드된 주소가 프로세스마다 다를 수 있기 때문에)
			// 그냥 클래스 static함수(혹은 전역함수)로 처리하기로 했다.

			MessagePack* message = reinterpret_cast<MessagePack*>(network->_recvBuffer);
			if (MessageType::LoginCTS == message->_messageType)
			{
				// 이때만 액터키가 어긋난다.. 서버에서는 이미 액터 만든상태고 클라는 아직 recv로 액터키 내려받지 못해서
				// 첫클라이언트인 경우 클라는 0 서버는 1(서버 0번은 리슨소켓)이된다.
				message->_networkActorKey = targetActorKey;
			}

			switch (message->_messageType)
			{

#include "Common/MessageTypeSwtichCaseSet.h"

				//case MessageType::LoginCTS:
				//	{
				//		LoginCTS::processMessage(static_cast<const LoginCTS*>(messagePack));
				//	}
				//	break;
			default:
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
				}
				break;
			}

			network->_prevRecvSize = 0;
			ZeroMemory(network->_recvBuffer, sizeof(network->_recvBuffer));
		}

		return rv;
	}
}


namespace ta
{
	const bool Socket::Startup(void) noexcept
	{
		WSADATA w;
		const bool isSucceeded = (0 == WSAStartup(MAKEWORD(2, 2), &w));
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "WSAStartup fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	const bool Socket::Cleanup(void) noexcept
	{
		const bool isSucceeded = (0 == WSACleanup());
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "WSACleanup fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	// 소켓을 생성하지는 않는다.
	Socket::Socket(void) noexcept
	{
		clear();
	}

	Socket::Socket(const SocketType socketType) noexcept
	{
		clear();
		// overlapped I/O를 쓰려면 socket() 말고 WSASocket을 써야 합니다.
		// TCP or UDP => AF_INET
		if (socketType == SocketType::Tcp)
		{
			// TCP는 스트림형식이다.
			_handle = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		}
		else // UDP
		{
			// UDP는 데이터그램 형식이다
			_handle = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		}
	}

	//Socket::Socket(const SOCKET& socket) noexcept
	//	: _handle(socket)
	//{
	//}

	Socket::~Socket(void)
	{
		close();
	}

	void Socket::setSocketHandle(const SOCKET& socket) noexcept
	{
		_handle = socket;
	}

	void Socket::clear(void) noexcept
	{
		static_assert(-1 == INVALID_SOCKET, "");
		_handle = -1;
		_prevRecvSize = 0;
		ZeroMemory(_recvBuffer, sizeof(_recvBuffer));
	}

	bool Socket::close(void) noexcept
	{
		const bool isSucceeded = (0 == ::closesocket(_handle));
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "closesocket fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	void Socket::disconnect(void) noexcept
	{
		close();
	}

	bool Socket::bind(const Endpoint& endpoint) noexcept
	{
		const bool isSucceeded = (0 == ::bind(_handle, (sockaddr*)&endpoint._ipv4Endpoint, sizeof(endpoint._ipv4Endpoint)));
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "bind fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	bool Socket::listen(void) noexcept
	{
		const bool isSucceeded = (0 == ::listen(_handle, MaxActorDataPoolCapacity));
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "listen fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	bool Socket::connectOverlapped(const Endpoint& endpoint) noexcept
	{
		if (NULL == _connectEx)
		{
			DWORD bytes = 0;

			GUID id = GUID(WSAID_CONNECTEX);

			WSAIoctl(_handle // 클라에서 커넥션하고싶은 소켓
					 , SIO_GET_EXTENSION_FUNCTION_POINTER // The control code of operation to perform. // acceptex사용하려면 무조건
					 , &id // A pointer to the input buffer. // acceptex사용하려면 무조건
					 , sizeof(GUID) // The size, in bytes, of the input buffer.
					 , &_connectEx // A pointer to the output buffer.
					 , sizeof(_connectEx) // The size, in bytes, of the output buffer.
					 , &bytes // The size, in bytes, of the output buffer.
					 , NULL // A pointer to actual number of bytes of output.
					 , NULL); // A pointer to a WSAOVERLAPPED structure (ignored for non-overlapped sockets).

			if (NULL == _connectEx)
			{
				TA_ASSERT_DEV(false, "Getting ConnectEx ptr failed.")
					return false;
			}
		}

		DWORD ignored = 0;
		_readOverlappedStruct.clear();
		_readOverlappedStruct._workType = ThreadWorkType::SocketConnect;

		const bool isSucceeded = _connectEx(_handle // 바운드O, 커넥션X
										   , (sockaddr*)&endpoint._ipv4Endpoint // 연결할 EndPoint
										   , sizeof(endpoint._ipv4Endpoint) // EndPoint크기
										   , NULL // 보낼 버퍼
										   , 0 // 보낼 버퍼 사이즈
										   , &ignored // 보낸 바이트수
										   , &_readOverlappedStruct._wsaOverlapped) == TRUE; // 요청처리하는데 사용될 오버랩 구조체

		if ((false == isSucceeded)
			&& (ERROR_IO_PENDING != ::GetLastError()))
		{
			TA_ASSERT_DEV(false, "_connectEx fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	bool Socket::updateConnectContext(void) noexcept
	{
		const bool isSucceeded = setsockopt(_handle
											, SOL_SOCKET
											, SO_UPDATE_CONNECT_CONTEXT
											, NULL
											, 0);

		return isSucceeded;
	}

	// 성공하면 true, 실패하면 false를 리턴합니다.
	// errorText에는 실패시 에러내용이 텍스트로  채워집니다.
	// acceptCandidateSocket에는 이미 만들어진 소켓 핸들이 들어가며, accept이 되고 나면 이 소켓 핸들은 TCP 연결 객체로 변신합니다.
	bool Socket::acceptOverlapped(void) noexcept
	{
		if (NULL == _acceptEx)
		{
			DWORD bytes;

			// AcceptEx는 여타 소켓함수와 달리 직접 호출하는 것이 아니고,
			// 함수 포인터를 먼저 가져온 다음 호출할 수 있다. 그것을 여기서 한다.

			/*
			AcceptEx함수는 런타임에 SIO_GET_EXTENSION_FUNCTION_POINTER, WSAID_ACCEPTEX 사용한 WSAIoctl을 호출해서 얻어와야 함
			*/

			GUID id = GUID(WSAID_ACCEPTEX);

			WSAIoctl(_handle // 리슨소켓
					 , SIO_GET_EXTENSION_FUNCTION_POINTER // The control code of operation to perform. // acceptex사용하려면 무조건
					 , &id // A pointer to the input buffer. // acceptex사용하려면 무조건
					 , sizeof(GUID) // The size, in bytes, of the input buffer.
					 , &_acceptEx // A pointer to the output buffer.
					 , sizeof(_acceptEx) // The size, in bytes, of the output buffer.
					 , &bytes // The size, in bytes, of the output buffer.
					 , NULL // A pointer to actual number of bytes of output.
					 , NULL); // A pointer to a WSAOVERLAPPED structure (ignored for non-overlapped sockets).

			if (NULL == _acceptEx)
			{
				TA_ASSERT_DEV(false, "Getting AcceptEx ptr failed.")
					return false;
			}
		}

		char ignored[200];
		DWORD ignored2 = 0;

		_readOverlappedStruct.clear();
		_readOverlappedStruct._acceptedClientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		_readOverlappedStruct._workType = ThreadWorkType::SocketAccept;

		const bool isSucceeded = _acceptEx(_handle // 리슨소켓
										   , _readOverlappedStruct._acceptedClientSocket // 예약걸어둘 소켓 // 바운드, 커넥션 X
										   , &ignored // 받는 데이터, 로컬주소, 리모트주소
										   , 0 // 받을 크기, 0전달시 받는 작업 X 데이터 안기다리고 바로 완료
										   , 50 // 로컬주소 정보 예약 바이트수
										   , 50 // 리모트주소 정보 예약 바이트수
										   , &ignored2 // 몇바이트 받았는지 반환한다.
										   , &_readOverlappedStruct._wsaOverlapped) == TRUE; // 요청처리하는데 사용될 오버랩 구조체

		if ((false == isSucceeded)
			&& (ERROR_IO_PENDING !=::GetLastError()))
		{
			TA_ASSERT_DEV(false, "_acceptEx fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	// AcceptEx가 I/O 완료를 하더라도 아직 TCP 연결 받기 처리가 다 끝난 것이 아니다.
	// 이 함수를 호출해주어야만 완료가 된다.
	bool Socket::updateAcceptContext(Socket& listenSocket) noexcept
	{
		sockaddr_in ignore1;
		sockaddr_in ignore3;
		INT ignore2, ignore4;

		char ignore[1000];
		// GetAcceptExSockaddrs로 3파트(데이터, 로컬 주소, 리모트 주소)로 파싱
		// 무조건 호출해주어야 한다
		GetAcceptExSockaddrs(ignore
							 , 0
							 , 50
							 , 50
							 , (sockaddr**)&ignore1
							 , &ignore2
							 , (sockaddr**)&ignore3
							 , &ignore4);

		/*
		AcceptEx함수가 리턴되면 소켓은 기본상태인데, 리슨소켓의 소켓특성을 상속받지 못한다.
		그때 setsockopt함수를 SO_UPDATE_ACCEPT_CONTEXT플래그로 호출해주면 된다.
		*/
		return TRUE == setsockopt(_handle // 받은 소켓
						  , SOL_SOCKET
						  , SO_UPDATE_ACCEPT_CONTEXT
						  , (char*)&listenSocket._handle // 리슨소켓
						  , sizeof(listenSocket._handle));
	}

	// overlapeed 수신을 겁니다. 즉 백그라운드로 수신 처리를 합니다.
	// 수신되는 데이터는 m_receiveBuffer에 비동기로 채워집니다.
	// 리턴값: WSARecv의 리턴값 그대로입니다.
	bool Socket::receiveOverlapped(void) noexcept
	{
		_readOverlappedStruct.clear();
		_readOverlappedStruct._workType = ThreadWorkType::SocketReceive;

		const int result = WSARecv(_handle
								   , &_readOverlappedStruct._wsaBuf
								   , 1
								   , NULL // Overlapped struct 쓰기 때문에 NULL
								   , &_readOverlappedStruct._flag//&_readFlags // 굳이 관련된 플래그 쓸필요가 없을듯 // 부분메시지 등
								   , &_readOverlappedStruct._wsaOverlapped
								   , NULL);

		const bool isSucceeded = (0 == result);
		if ((false == isSucceeded)
			&& (ERROR_IO_PENDING != ::GetLastError()))
		{
			TA_ASSERT_DEV(false, "WSARecv fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	bool Socket::sendOverlapped(MessagePack* packet) noexcept
	{
		// 가상함수 있고 그래서 정확한 바이트 수가 안나온다.
		//char* buffer = reinterpret_cast<char*>(packet);

		// 일단 동적할당 처리했다. 완료되면 삭제한다.
		CustomOverlappedStruct* sendOverlappedStruct = new CustomOverlappedStruct;

		// 가상함수 있고 그래서 정확한 바이트 수가 안나온다.
		//memcpy(sendOverlappedStruct->_iocpBuf, buffer, (int)buffer[0]);
		memcpy(sendOverlappedStruct->_iocpBuf, packet, packet->_length);
		sendOverlappedStruct->_wsaBuf.len = packet->_length;
		sendOverlappedStruct->_workType = ThreadWorkType::SocketSend;

		const int result = WSASend(_handle
								   , &(sendOverlappedStruct->_wsaBuf)
								   , 1
								   , NULL // Overlapped struct 쓰기 때문에 NULL
								   , 0//&_readFlags // 굳이 관련된 플래그 쓸필요가 없을듯 // 부분메시지 등
								   , &(sendOverlappedStruct->_wsaOverlapped)
								   , NULL);

		const bool isSucceeded = (0 == result);
		if ((false == isSucceeded)
			&& (ERROR_IO_PENDING != ::GetLastError()))
		{
			TA_ASSERT_DEV(false, "WSASend fail : %s", GetLastErrorAsString().data());
		}

		return isSucceeded;
	}

	bool Socket::getPeerAddr(Endpoint& output) const noexcept
	{
		output.reset();
		socklen_t retLength = sizeof(output._ipv4Endpoint);
		const bool isSucceeded = (0 == ::getpeername(_handle, (sockaddr*)&output._ipv4Endpoint, &retLength));
		if (false == isSucceeded)
		{
			TA_ASSERT_DEV(false, "getPeerAddr fail : %s", GetLastErrorAsString().data());
			return false;
		}
		else
		{
			if (retLength > sizeof(output._ipv4Endpoint))
			{
				TA_ASSERT_DEV(false, "getPeerAddr buffer overrun : %d", retLength);
				return false;
			}
		}

		return true;
	}
}


namespace ta
{
	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	// 출처: https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
	tstring GetLastErrorAsString(void) noexcept
	{
		//Get the error message, if any.
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0)
			return tstring(); //No error message has been recorded

		TCHAR* messageBuffer = nullptr;

		size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR*) (&messageBuffer), 0, NULL);
		
		//CHAR* messageBuffer1 = nullptr;
		//size_t size1 = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		//							 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer1, 0, NULL);

		tstring message(messageBuffer, size);
		
		//Free the buffer.
		LocalFree(messageBuffer);


		return message;
	}
}
