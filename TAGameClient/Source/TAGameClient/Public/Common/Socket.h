#pragma once

#include "Common/CommonBase.h"
#include <winsock2.h> // Overlapped I/O
#include <mswsock.h> // Ex function

#include <string>
#include <vector>
#include "Common/KeyDefinition.h"


namespace ta
{
	class Endpoint;
	class Iocp;
	class CommonActor;
	class MessagePack;
}


namespace ta
{
	extern tstring GetLastErrorAsString(void) noexcept;
	extern bool ProcessReceive(const ActorKey& targetActorKey, const DWORD numberOfBytesTransferred) noexcept;
}


namespace ta
{
	class OverlappedStructBase
	{
	public:
		OverlappedStructBase(void) noexcept
		{
			clearBase();
		}

		void clearBase(void) noexcept
		{
			ZeroMemory(&_wsaOverlapped, sizeof(_wsaOverlapped));
			_workType = ThreadWorkType::Count;
		}

	public:
		WSAOVERLAPPED _wsaOverlapped;
		ThreadWorkType _workType;
	};
}


namespace ta
{
	class CustomOverlappedStruct : public OverlappedStructBase
	{
	public:
		CustomOverlappedStruct(void) noexcept
		{
			clear();
		}

		void clear(void) noexcept
		{
			OverlappedStructBase::clearBase();
			ZeroMemory(&_wsaBuf, sizeof(_wsaBuf));
			ZeroMemory(&_iocpBuf, sizeof(_iocpBuf));
			_wsaBuf.buf = _iocpBuf;
			_wsaBuf.len = MaxPacketBufferLength;
			_flag = 0;

			static_assert(-1 == INVALID_SOCKET, "");
			_acceptedClientSocket = -1;
		}


	public:
		WSABUF _wsaBuf;
		char _iocpBuf[MaxPacketBufferLength];
		DWORD _flag;

		// 리슨소켓에서만 의미 있음
		SOCKET _acceptedClientSocket;
	};
}


namespace ta
{
	class Socket
	{
	public:
		// 애플리케이션을 시작할때 한번호출되고 // Winsock DLL Use관련
		static const bool Startup(void) noexcept;
		// 에플리케이션을 종료할때 한번호출되어야한다.
		static const bool Cleanup(void) noexcept;

	public:
		Socket(void) noexcept;
		explicit Socket(const SocketType socketType) noexcept;
		//explicit Socket(const SOCKET& socketHandle) noexcept;
		~Socket(void);

		void setSocketHandle(const SOCKET& socketHandle) noexcept;

		void clear(void) noexcept;

		bool close(void) noexcept;
		void disconnect(void) noexcept;

		bool bind(const Endpoint& endpoint) noexcept;
		bool listen(void) noexcept;

		//////////////////////Client//////////////////////////

		bool connectOverlapped(const Endpoint& endpoint) noexcept;
		bool updateConnectContext(void) noexcept;
		//////////////////////Client//////////////////////////

		//////////////////////Server//////////////////////////

		// for listen socket
		bool acceptOverlapped(void) noexcept;
		// for remote client socket
		bool updateAcceptContext(Socket& listenSocket) noexcept;

		//////////////////////Server//////////////////////////

		bool receiveOverlapped(void) noexcept;
		bool sendOverlapped(MessagePack* packet) noexcept;

		bool getPeerAddr(Endpoint& input) const noexcept;

	public:
		SOCKET _handle; // 소켓 핸들

		// receiveOverlapped / acceptOverlapped 에 의해 수신되는 데이터가 채워지는 곳입니다.
		// overlapped receive를 하는 동안 여기가 사용됩니다. overlapped I/O가 진행되는 동안 이 값을 건드리지 마세요.
		CustomOverlappedStruct _readOverlappedStruct;

		// 잘려서 들어올때를 대비해서 recv에 필요한 임시버퍼
		uint32 _prevRecvSize;
		char _recvBuffer[MaxPacketBufferLength];

	private:
		// overlapped I/O , iocp 사용
		LPFN_ACCEPTEX _acceptEx = NULL;
		LPFN_CONNECTEX _connectEx = NULL;

		/*
		같은 소켓에 대해서 다른스레드에서 동시에 send를 호출하면 데이터 인터리빙이 발생할 수 있음
		send 요청 큐가 차있으면 WSA버퍼배열에 복사해서 한꺼번에 보내는 방식으로 사용 (N-send) 이 경우 순서는 보장된다.
		CustomOverlappedData에 몇개의 WSA버퍼를 사용하는지도 같이 넣어주자
		*/


	};
}


