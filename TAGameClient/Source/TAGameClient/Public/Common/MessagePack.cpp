#include "Common/MessagePack.h"
#include "Common/CommonApp.h"
#include "Common/CommonActorManager.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	StringPack::StringPack(void) noexcept
	{
		memset(_data, NULL, sizeof(_data));
	}

	StringPack::StringPack(const char* data) noexcept
	{
		copy(data);
	}

	StringPack::StringPack(const StringPack& pack) noexcept
	{
		copy(pack);
	}
	
	StringPack::~StringPack(void) noexcept
	{
	}
	
	StringPack& StringPack::operator=(const StringPack& pack) noexcept
	{
		copy(pack);
		return *this;
	}

	StringPack& StringPack::operator=(const char* data) noexcept
	{
		copy(data);
		return *this;
	}

	tstring StringPack::getAsTstring(void) const noexcept
	{
		TCHAR data[MaxStringPackBufferLength]{NULL,};

		CharToTChar(_data, data, MaxStringPackBufferLength);
		return tstring(data);
	}

	void StringPack::copy(const StringPack& pack) noexcept
	{
		uint32 destinationSize = sizeof(_data);
		memcpy(_data, pack._data, destinationSize);
		_data[destinationSize - 1] = NULL;
	}

	void StringPack::copy(const char* data) noexcept
	{
		uint32 destinationSize = sizeof(_data);
		strncpy_s(_data, data, destinationSize);
		_data[destinationSize - 1] = NULL;
	}
}

namespace ta
{
	MessagePack::MessagePack(const MessageType& type) noexcept
		: _messageType(type)
		, _length(0)
		, _networkActorKey(0)
	{
	}

	MessagePack::~MessagePack(void) noexcept
	{
	}
}
