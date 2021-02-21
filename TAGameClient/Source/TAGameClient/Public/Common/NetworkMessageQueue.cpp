#include "Common/NetworkMessageQueue.h"
#include "Common/CommonBase.h"
#include "Common/MessagePack.h"
#include "Common/ScopedLock.h"



namespace ta
{
    NetworkMessageQueue::NetworkMessageQueue(const uint32 capacity) noexcept
        : _capacity(capacity)
    {
        if (false == open())
        {
            TA_ASSERT_DEV(false, "NetworkMessageQueue open에 실패했습니다.");
        }
    }

    NetworkMessageQueue::~NetworkMessageQueue(void) noexcept
    {
        close();
    }

    bool NetworkMessageQueue::enqueueMessage(const std::shared_ptr<MessagePack>& msg) noexcept
    {
        ScopedLock lock(this);

        if (nullptr == msg.get())
        {
            TA_ASSERT_DEV(false, "메시지가 없습니다.");
            return false;
        }

        _messagePackQueue.push_back(msg);
        return true;
    }

    std::shared_ptr<MessagePack> NetworkMessageQueue::dequeueMessage(void) noexcept
    {
        ScopedLock lock(this);

        const uint32 count = _messagePackQueue.size(); 
        if (0 == count)
        {
            return nullptr;
        }

        std::shared_ptr<MessagePack> returnValue = _messagePackQueue[0];
        _messagePackQueue.erase(_messagePackQueue.begin());

        return returnValue;
    }

    bool NetworkMessageQueue::open(void) noexcept
    {
        ScopedLock lock(this);

        _messagePackQueue.reserve(_capacity);
        return true;
    }

    void NetworkMessageQueue::close(void) noexcept
    {
        ScopedLock lock(this);

        _messagePackQueue.clear();
    }
}


