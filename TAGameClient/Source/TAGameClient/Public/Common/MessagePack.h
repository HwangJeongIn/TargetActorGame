#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/MessageTypeDefinition.h"					  
#include "Common/ComponentData.h"


namespace ta
{
	class CommonActor;
}


namespace ta
{
//#pragma pack(push, 1)
	class StringPack
	{
	public:
		StringPack(void) noexcept;
		explicit StringPack(const char* data) noexcept;
		explicit StringPack(const StringPack& pack) noexcept;
		virtual ~StringPack(void) noexcept;
		StringPack& operator=(const StringPack & pack) noexcept;
		StringPack& operator=(const char* data) noexcept;

		tstring getAsTstring(void) const noexcept;

	private:
		void copy(const StringPack& pack) noexcept;
		void copy(const char* data) noexcept;

	public:
		char _data[MaxStringPackBufferLength];
	};
}


namespace ta
{
//#pragma pack(push, 1)
	class MessagePack
	{
	public:
		explicit MessagePack(const MessageType& type) noexcept;
		virtual ~MessagePack(void) noexcept;

	public:
		uint32 _length;
		MessageType _messageType;
		ActorKey _networkActorKey; // 안쓰려 했는데, 패킷함수 구현중 해당 액터키가 필요해서 살려두기로 했다.
	};
}
