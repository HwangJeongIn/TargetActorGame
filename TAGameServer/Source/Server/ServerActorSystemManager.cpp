#include "Server/ServerActorSystemManager.h"
#include "Server/ServerMoveActorSystem.h"
#include "Server/ServerActionActorSystem.h"
#include "Server/ServerAiActorSystem.h"
#include "Server/ServerInventoryActorSystem.h"


namespace ta
{
    ServerActorSystemManager::ServerActorSystemManager(CommonActorManager* actorManager) noexcept
        : CommonActorSystemManager(actorManager)
    {
    }

    ServerActorSystemManager::~ServerActorSystemManager(void) noexcept
    {
    }
    
    bool ServerActorSystemManager::initialize(void) noexcept
    {
        if (false == __super::initialize())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        uint32 systemCount = static_cast<uint32>(ActorSystemType::Count);
        _actorSystems.reserve(systemCount);

        for (uint32 index = 0; index < systemCount; ++index)
        {
            const ActorSystemType systemType = static_cast<ActorSystemType>(index);

            switch (systemType)
            {

#define CREATE_SYSTEM(SystemType, SystemName)                 \
            case ActorSystemType::SystemType:                 \
                {                                             \
                    _actorSystems.push_back(new SystemName);  \
                }                                             \
                break;


             CREATE_SYSTEM(ActionSystem, ServerActionActorSystem)
             CREATE_SYSTEM(MoveSystem, ServerMoveActorSystem)
             CREATE_SYSTEM(AiSystem, ServerAiActorSystem)
            // CREATE_SYSTEM(CharacterSystem, ServerCharacterActorSystem)
             CREATE_SYSTEM(InventorySystem, ServerInventoryActorSystem)

#undef CREATE_SYSTEM

             TA_COMPILE_DEV(4 == static_cast<uint8>(ActorSystemType::Count), "여기도 추가해주세요");

            default:
                {
                    TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
                }
                break;
            }
        }

        return true;
    }

    bool ServerActorSystemManager::open(void) noexcept
    {
        if (false == __super::open())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }
        return true;
    }

    void ServerActorSystemManager::close(void) noexcept
    {
        __super::close();
    }
}

