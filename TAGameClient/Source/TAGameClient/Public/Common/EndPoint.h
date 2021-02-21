#pragma once

#include "Common/CommonBase.h"
#include <Ws2tcpip.h>
#include <string>


namespace ta
{
	class Endpoint
	{
	public:
		Endpoint(void) noexcept;
		explicit Endpoint(const char* address, const int port) noexcept;
		~Endpoint(void) noexcept;

		void reset(void) noexcept;

		sockaddr_in _ipv4Endpoint;
		std::string toString();
		tstring toTstring();
	};
}


