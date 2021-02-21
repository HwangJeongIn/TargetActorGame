#pragma once

#include "Common/CommonBase.h"
#include <vector>
#include "Common/Lockable.h"


namespace ta
{
	class MessagePack;
}


namespace ta
{
	class NetworkMessageQueue : public Lockable
	{
	public:
		NetworkMessageQueue(const uint32 capacity = 32) noexcept;
		virtual ~NetworkMessageQueue(void) noexcept;

		bool enqueueMessage(const std::shared_ptr<MessagePack>& msg) noexcept;
		std::shared_ptr<MessagePack> dequeueMessage(void) noexcept;

	private:
		bool open(void) noexcept;
		void close(void) noexcept;

	private:

		std::vector<std::shared_ptr<MessagePack>> _messagePackQueue;
		uint32 _capacity;
	};
}
