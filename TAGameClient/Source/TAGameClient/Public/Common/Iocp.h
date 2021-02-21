#pragma once

#include "Common/CommonBase.h"
#include <winsock2.h> // Overlapped I/O
#include <mswsock.h> // Ex function


namespace ta
{
	class Socket;
	class ActorKey;
}


namespace ta
{
	class Iocp
	{
	public:
		Iocp(void) noexcept;
		bool bindSocket(Socket& socket,  const ActorKey& actorKey) noexcept;

	public:
		HANDLE _handle;
	};
}