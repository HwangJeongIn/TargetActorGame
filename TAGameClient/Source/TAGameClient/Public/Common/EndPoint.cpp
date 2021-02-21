
#pragma comment( lib, "ws2_32.lib")

#include "Common/Endpoint.h"
#include <cstdint>



namespace ta
{
	Endpoint::Endpoint(void) noexcept
	{
		// 커널이 알아서 해줌
		memset(&_ipv4Endpoint, 0, sizeof(_ipv4Endpoint));
		_ipv4Endpoint.sin_family = AF_INET; // ipv4 endpoint
	}

	// address, port를 입력받아 endpoint를 준비한다.
	Endpoint::Endpoint(const char* address, const int port) noexcept
	{
		memset(&_ipv4Endpoint, 0, sizeof(_ipv4Endpoint)); // 초기화
		_ipv4Endpoint.sin_family = AF_INET; // ipv4 endpoint로 사용
		inet_pton(AF_INET, address, &_ipv4Endpoint.sin_addr); // ipv4 문자열을 IN_ADDR로 변환
		_ipv4Endpoint.sin_port = htons((uint16_t)port); // 네트워크 바이트 오더인 빅엔디안에 맞춰 변환하여 초기화한다.
	}

	Endpoint::~Endpoint(void) noexcept
	{
	}

	void Endpoint::reset(void) noexcept
	{
		memset(&_ipv4Endpoint, 0, sizeof(_ipv4Endpoint));
		_ipv4Endpoint.sin_family = AF_INET; // ipv4 endpoint
	}

	std::string Endpoint::toString()
	{
		char addrString[1000];
		addrString[0] = 0;
		inet_ntop(AF_INET, &_ipv4Endpoint.sin_addr, addrString, sizeof(addrString) - 1); // 스트링 포맷으로 변경

		char finalString[1000];
		sprintf_s(finalString, 1000, "%s:%d", addrString, htons(_ipv4Endpoint.sin_port));

		return finalString;
	}

	tstring Endpoint::toTstring()
	{
		TCHAR data[MaxStringPackBufferLength]{ NULL, };

		CharToTChar(toString().data(), data);
		return tstring(data);
	}
}

