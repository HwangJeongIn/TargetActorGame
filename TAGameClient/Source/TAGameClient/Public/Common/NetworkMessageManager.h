#pragma once


namespace ta
{
	class NetworkMessageQueue;
}


namespace ta
{
	class NetworkMessageManager
	{

	public:
		NetworkMessageManager(void) noexcept;
		virtual ~NetworkMessageManager(void) noexcept;


	private:
		NetworkMessageQueue* _receivingMessageQueue;
		NetworkMessageQueue* _sendingMessageQueue;


	};
}
