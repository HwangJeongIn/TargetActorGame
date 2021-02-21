#include "Common/NetworkMessageManager.h"
#include "Common/NetworkMessageQueue.h"


namespace ta
{
	NetworkMessageManager::NetworkMessageManager(void) noexcept
		: _receivingMessageQueue(nullptr)
		, _sendingMessageQueue(nullptr)
	{
		_receivingMessageQueue = new NetworkMessageQueue();
		_sendingMessageQueue = new NetworkMessageQueue();
	}

	NetworkMessageManager::~NetworkMessageManager(void) noexcept
	{
		if (nullptr != _receivingMessageQueue)
		{
			delete _receivingMessageQueue;
		}

		if (nullptr != _sendingMessageQueue)
		{
			delete _sendingMessageQueue;
		}
	}
}

