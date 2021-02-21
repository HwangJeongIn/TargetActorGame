#include "Client/ClientActorSystemManager.h"
#include "Client/ClientMoveActorSystem.h"
#include "Client/ClientActionActorSystem.h"
#include "Client/ClientAiActorSystem.h"
#include "Client/ClientInventoryActorSystem.h"


namespace ta
{
    ClientActorSystemManager::ClientActorSystemManager(CommonActorManager* actorManager) noexcept
        : CommonActorSystemManager(actorManager)
    {
    }

    ClientActorSystemManager::~ClientActorSystemManager(void) noexcept
    {
    }

    bool ClientActorSystemManager::initialize(void) noexcept
    {
        if (false == CommonActorSystemManager::initialize())
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


                CREATE_SYSTEM(ActionSystem, ClientActionActorSystem)
                CREATE_SYSTEM(MoveSystem, ClientMoveActorSystem)
                CREATE_SYSTEM(AiSystem, ClientAiActorSystem)
                //CREATE_SYSTEM(CharacterSystem, ClientCharacterActorSystem)
                CREATE_SYSTEM(InventorySystem, ClientInventoryActorSystem)

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

    bool ClientActorSystemManager::open(void) noexcept
    {
        if (false == CommonActorSystemManager::open())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }
        return true;
    }

    void ClientActorSystemManager::close(void) noexcept
    {
        CommonActorSystemManager::close();
    }
}

