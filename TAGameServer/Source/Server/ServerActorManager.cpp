#include "Server/ServerActorManager.h"
#include "Server/ServerActorDataPool.h"
#include "Server/ServerComponentData.h"
#include "Server/ServerSectors.h"
#include "Server/ServerActor.h"
#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerActionActorComponent.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/ServerCharacterActorComponent.h"
#include "Server/ServerInventoryActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/ActorDataGroups.h"
#include "Common/ComponentData.h"
#include "Common/GetComponentAndSystem.h"
#include "ServerActorManager.h"


namespace ta
{
    ServerActorManager::ServerActorManager(void) noexcept
    {
    }

    ServerActorManager::~ServerActorManager(void) noexcept
    {
    }
    
    bool ServerActorManager::initialize(void) noexcept
    {
        if (false == CommonActorManager::initialize())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        _actorDataPool = new ServerActorDataPool;
        _sectors = new ServerSectors;

        if (false == doInitialize())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        return true;
    }

    bool ServerActorManager::open(void) noexcept
    {
        if (false == CommonActorManager::open())
        {
            TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
            return false;
        }

        return true;
    }

    void ServerActorManager::close(void) noexcept
    {
        CommonActorManager::close();
    }

    //CommonActor* ServerActorManager::createActorAndInitializeFromDB(const ActorType& actorType, const CharacterDBNo& characterNo) noexcept
    //{
    //    ServerActor* newActor = static_cast<ServerActor*>(createActorForServer(actorType));
    //    if (nullptr == newActor)
    //    {
    //        TA_ASSERT_DEV(false, "비정상입니다.");
    //        return nullptr;
    //    }

    //    if (false == initializeActorComponentsFromDB(newActor->getActorKey(), characterNo))
    //    {
    //        TA_ASSERT_DEV(false, "비정상입니다.");
    //        return nullptr;
    //    }

    //    return newActor;
    //}

    CommonActor* ServerActorManager::createCharacterActorAndInitializeComponentsFromSpawnData(const CommonActorDetailSpawnDataForCharacter& detailSpawnData) noexcept
    {
        const GroupGameData* groupGameData = GetGameData<GroupGameData>(detailSpawnData._groupGameDataKey);
        if (nullptr == groupGameData)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        const CharacterGameData* characterGameData = groupGameData->_characterGameData;

        if (ActorType::Player == characterGameData->_actorType)
        {
            TA_ASSERT_DEV(false, "플레이어는 스폰데이터로 스폰될수없습니다.");
            return nullptr;
        }

        CommonActor* newActor = createActorForServer(characterGameData->_actorType);
        if (nullptr == newActor)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        if (false == initializeActorComponentsFromData(newActor->getActorKey(), detailSpawnData))
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        return newActor;
    }

    CommonActor* ServerActorManager::createActorForServer(const ActorType& actorType) noexcept
    {
        switch (actorType)
        {
        //case ActorType::Default:
        //    {
        //        return createDefaultActorForServer();
        //    }
        //    break;
        case ActorType::Player:
            {
                return createPlayerActorForServer();
            }
            break;
        case ActorType::ControlledByPlayer:
            {
                //return createControlledByPlayerActorForServer();
            }
            break;
        //case ActorType::Monster:
        //    {
        //        return createMonsterActorForServer();
        //    }
        //    break;
        case ActorType::Npc:
            {
                return createNpcActorForServer();
            }
            break;
        default:
            {
                TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 추가해주세요");
            }
            break;
        }

        return nullptr;
    }

    //CommonActor* ServerActorManager::createDefaultActorForServer(void) noexcept
    //{
    //    CommonActorBasicSpawnData spawnData;
    //    spawnData.initializeComponentsAsDefaultActor();
    //    return _actorDataPool->fromPool(spawnData);
    //}

    CommonActor* ServerActorManager::createPlayerActorForServer(void) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsPlayer();
        spawnData._networkOwnerType = NetworkOwnerType::Mine;
        return _actorDataPool->fromPool(spawnData);
    }

    //CommonActor* ServerActorManager::createMonsterActorForServer(void) noexcept
    //{
    //    CommonActorBasicSpawnData spawnData;
    //    spawnData.initializeComponentsAsMonster();
    //    return _actorDataPool->fromPool(spawnData);
    //}

    CommonActor* ServerActorManager::createNpcActorForServer(void) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsNpc();
        return _actorDataPool->fromPool(spawnData);
    }

    bool ServerActorManager::initializeActorComponentsFromDB(const ActorKey& targetActorKey, const CharacterDBNo& characterDBNo) noexcept
    {
        ServerActor* serverActor = static_cast<ServerActor*>(getActor(targetActorKey));
        if (nullptr == serverActor)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }        
        
        if (false == characterDBNo.isValid())
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }

        {
            ScopedLock actorLock(serverActor);
            serverActor->setCharacterDBNo_(characterDBNo);
        }

        // 임시로 그룹키 1로 스폰시킨다.
        GroupGameDataKey groupGameDataKey(1);
        const GroupGameData* groupGameData = GetGameData<GroupGameData>(groupGameDataKey);
        if (nullptr == groupGameData)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }

        CommonActorDetailSpawnDataForUser spawnDataForUser(Vector(-37282, 13528, 6477), Vector(0, 0, 0), GroupGameDataKey(1));

        return initializeActorComponentsFromData(targetActorKey, spawnDataForUser);
    }

    CommonActor* ServerActorManager::createCharacterActorAndInitializeComponentsForServer(const CommonActorDetailSpawnDataForCharacter& detailSpawnData) noexcept
    {
        return createCharacterActorAndInitializeComponentsFromSpawnData(detailSpawnData);
    }

    bool ServerActorManager::initializeActorComponentsFromData(const ActorKey& targetActorKey, const CommonActorDetailSpawnData& detailSpawnData) noexcept
    {
        ServerActor* serverActor = static_cast<ServerActor*>(getActor(targetActorKey));
        if (nullptr == serverActor)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }

        const ActorType spawnDataActorType = detailSpawnData.getActorType();
        const GroupGameData* groupGameData = nullptr;
        switch (spawnDataActorType)
        {
        case ActorType::Player:
        case ActorType::Npc:
            {
                const CommonActorDetailSpawnDataForCharacter& temp = static_cast<const CommonActorDetailSpawnDataForCharacter&>(detailSpawnData);
                if (false == temp._groupGameDataKey.isValid())
                {
                    TA_ASSERT_DEV(false, "비정상입니다.");
                    return false;
                }

                groupGameData = GetGameData<GroupGameData>(temp._groupGameDataKey);
                if (nullptr == groupGameData)
                {
                    TA_ASSERT_DEV(false, "데이터가 없습니다. GroupGameDataKey : %d", temp._groupGameDataKey.getKeyValue());
                    return false;
                }
            }
            break;
        case ActorType::Object:
            {
                __noop;
            }
            break;
        default:
            {
                TA_COMPILE_DEV(4 == static_cast<uint8>(ActorType::Count), "여기도 추가해주세요");
            }
            break;
        }

        ActorType actorType = serverActor->getActorType();
        std::unordered_map<ActorType, ActorGroup>::const_iterator it = ActorDataGroups.find(actorType);
        if (ActorDataGroups.end() == it)
        {
            TA_ASSERT_DEV(false, "비정상입니다");
            return false;
        }

        const std::vector<ActorComponentType>& componentTypeList = it->second._componentTypeList;

        // TODO : 유저인 경우 DB + 데이터로부터 로드해야합니다.
        const uint32 count = componentTypeList.size();
        for (uint32 index = 0; index < count; ++index)
        {
            switch (componentTypeList[index])
            {
            case ActorComponentType::Move:
                {  
                    CommonMoveComponentData data;
                    data._position = detailSpawnData._position;
                    data._rotation = detailSpawnData._rotation;
                    data._speed = groupGameData->_moveGameData->_speed;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Action:
                {
                    CommonActionComponentData data;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Ai:
                {
                    const AiGameData* aiGameData = groupGameData->_aiGameData;
                    CommonAiComponentData data;
                    if (nullptr != aiGameData) // 없을 수 있다.
                    {
                        data._aiClassType = aiGameData->_aiClassType;
                        data._pathPointPathKey = aiGameData->_pathPointPathKey;
                    }
                   
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }

                }
                break;
            case ActorComponentType::Character:
                {
                    const CharacterGameData* characterGameData = groupGameData->_characterGameData;

                    CommonCharacterComponentData data;
                    data._currentHp = characterGameData->_maxHp;
                    data._characterGameDataKey = characterGameData->_key;

                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Inventory:
                {
                    if (ActorType::Player != spawnDataActorType)
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }

                    CommonInventoryComponentData data;

                    // 임시
                    {
                        data._itemSetType = ItemSetType::ContainerType;
                        data._capacity = 15;

                        ItemElementData itemData;
                        itemData._baseKey = ItemGameDataKey(1);
                        itemData._stackCount = 2;
                        data._itemElementDataSet[1] = itemData;


                        itemData._baseKey = ItemGameDataKey(2);
                        itemData._stackCount = 3;
                        data._itemElementDataSet[3] = itemData;
                    }

                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Object:
                {
                    if (ActorType::Object != spawnDataActorType)
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }

                    const CommonActorDetailSpawnDataForObject& temp = static_cast<const CommonActorDetailSpawnDataForObject&>(detailSpawnData);

                    CommonObjectComponentData data;
                    data._itemGameDataKey = temp._itemGameDataKey;
                    data._renderingGameDataKey = temp._renderingGameDataKey;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            default:
                {
                    TA_COMPILE_DEV((6 == static_cast<uint8>(ActorComponentType::Count)), "여기도 추가해주세요")
                }
                break;
            }

        }

        return true;
    }


}

