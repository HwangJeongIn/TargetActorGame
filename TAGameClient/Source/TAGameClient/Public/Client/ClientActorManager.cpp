#include "Client/ClientActorManager.h"
#include "Client/ClientActorDataPool.h"
#include "Client/ClientComponentData.h"
#include "Client/ClientSector.h"


namespace ta
{
    ClientActorManager::ClientActorManager(void) noexcept
    {
    }

    ClientActorManager::~ClientActorManager(void) noexcept
    {
    }

    bool ClientActorManager::initialize(void) noexcept
    {
        if (false == CommonActorManager::initialize())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        _allSectors = new ClientSector[CountOfSectors];

        _actorDataPool = new ClientActorDataPool;
        if (false == _actorDataPool->initialize())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        return true;
    }

    bool ClientActorManager::open(void) noexcept
    {
        if (false == CommonActorManager::open())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        for (uint32 index = 0; index < CountOfSectors; ++index)
        {
            _allSectors[index].setSectorKey(SectorKey(index));
        }

        if (false == _actorDataPool->open())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        return true;
    }

    void ClientActorManager::close(void) noexcept
    {
        _actorDataPool->close();

        delete[] _allSectors;

        CommonActorManager::close();
    }

    Sector* ClientActorManager::getSector(const SectorKey& sectorKey) noexcept
    {
        if (false == checkSectorValid(sectorKey))
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        const int32 index = sectorKey.getKeyValue();
        ClientSector* indexPtr = static_cast<ClientSector*>(_allSectors);

        return &(indexPtr[index]);
    }

    CommonActor* ClientActorManager::createActorForClient(const ActorType& actorType, const ActorKey& actorKeyReceived) noexcept
    {
        switch (actorType)
        {
        case ActorType::Default:
            {
                return createDefaultActorForClient(actorKeyReceived);
            }
            break;
        case ActorType::Player:
            {
                return createPlayerActorForClient(actorKeyReceived);
            }
            break;
        case ActorType::ControlledByPlayer:
            {
                return createControlledByPlayerActorForClient(actorKeyReceived);
            }
            break;
        case ActorType::Monster:
            {
                return createMonsterActorForClient(actorKeyReceived);
            }
            break;
        case ActorType::Npc:
            {
                return createNpcActorForClient(actorKeyReceived);
            }
            break;
        default:
            {
                TA_COMPILE_DEV(6 == static_cast<uint8>(ActorType::Count), "여기도 추가해주세요");
            }
            break;
        }

        return nullptr;
    }

    CommonActor* ClientActorManager::createDefaultActorForClient(const ActorKey& actorKeyReceived) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsDefaultActor();
        return _actorDataPool->fromPool(spawnData, true, actorKeyReceived);
    }

    CommonActor* ClientActorManager::createPlayerActorForClient(const ActorKey& actorKeyReceived) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsPlayer();
        spawnData._networkOwnerType = NetworkOwnerType::BorrowingNetwork;
        return _actorDataPool->fromPool(spawnData,true, actorKeyReceived);
    }

    CommonActor* ClientActorManager::createControlledByPlayerActorForClient(const ActorKey& actorKeyReceived) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsControlledByPlayer();
        spawnData._networkOwnerType = NetworkOwnerType::BorrowingNetwork;
        return _actorDataPool->fromPool(spawnData, true, actorKeyReceived);
    }
    
    CommonActor* ClientActorManager::createMonsterActorForClient(const ActorKey& actorKeyReceived) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsMonster();
        return _actorDataPool->fromPool(spawnData, true, actorKeyReceived);
    }

    CommonActor* ClientActorManager::createNpcActorForClient(const ActorKey& actorKeyReceived) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsNpc();
        return _actorDataPool->fromPool(spawnData, true, actorKeyReceived);
    }
    
    const ActorKey ClientActorManager::getMainActorKeyWithCompletionKey(const ActorKey& completionKey) noexcept
    {
        // 클라이언트는 OwnerActorKey로 컴플리션키가 들어온다.
        const ActorKey ownerActorKey = getOwnerActorKey();
        TA_ASSERT_DEV((completionKey == ownerActorKey), "앱 초기화가 비정상입니다.")

        return (true == _mainActorKey.isValid()) ? _mainActorKey : ownerActorKey;
    }
    
    bool ClientActorManager::bindActorAsMainActor(const ActorKey& targetActorKey) noexcept
    {
        if (true == _mainActorKey.isValid())
        {
            TA_ASSERT_DEV(false, "두번 초기화하려합니다.")
            return false;
        }

        _mainActorKey = targetActorKey;
        return true;
    }
}

