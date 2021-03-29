#include "Server/ServerActorManager.h"
#include "Server/ServerActorDataPool.h"
#include "Server/ServerComponentData.h"
#include "Server/ServerSector.h"
#include "Server/ServerActor.h"
#include "Server/ServerMoveActorComponent.h"
#include "Server/ServerActionActorComponent.h"
#include "Server/ServerAiActorComponent.h"
#include "Server/ServerCharacterActorComponent.h"
#include "Server/ServerInventoryActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/ActorComponentTypeList.h"
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
        _allSectors = new ServerSector[CountOfSectors];
        _actorDataPool = new ServerActorDataPool;

        if (false == CommonActorManager::initialize())
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

    Sector* ServerActorManager::getSector(const SectorKey& sectorKey) noexcept
    {
        if (false == checkSectorValid(sectorKey))
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        const int32 index = sectorKey.getKeyValue();
        ServerSector* indexPtr = static_cast<ServerSector*>(_allSectors);

        return &(indexPtr[index]);
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

    CommonActor* ServerActorManager::createActorAndInitializeFromSpawnData(const CommonActorDetailSpawnData& detailSpawnData) noexcept
    {
        const GroupGameData* groupGameData = GetGameData<GroupGameData>(detailSpawnData._groupGameDataKey);
        if (nullptr == groupGameData)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

        const CharacterGameData* characterGameData = GetGameData<CharacterGameData>(groupGameData->_characterGameDataKey);
        if (nullptr == characterGameData)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return nullptr;
        }

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
        case ActorType::Default:
            {
                return createDefaultActorForServer();
            }
            break;
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
        case ActorType::Monster:
            {
                return createMonsterActorForServer();
            }
            break;
        case ActorType::Npc:
            {
                return createNpcActorForServer();
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

    CommonActor* ServerActorManager::createDefaultActorForServer(void) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsDefaultActor();
        return _actorDataPool->fromPool(spawnData);
    }

    CommonActor* ServerActorManager::createPlayerActorForServer(void) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsPlayer();
        spawnData._networkOwnerType = NetworkOwnerType::Mine;
        return _actorDataPool->fromPool(spawnData);
    }

    CommonActor* ServerActorManager::createMonsterActorForServer(void) noexcept
    {
        CommonActorBasicSpawnData spawnData;
        spawnData.initializeComponentsAsMonster();
        return _actorDataPool->fromPool(spawnData);
    }

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

        ActorType actorType = ActorType::Count;
        {
            ScopedLock actorLock(serverActor, true);
            actorType = serverActor->getActorType_();
        }

        const std::vector<ActorComponentType>& componentTypeList = ActorComponentGroups.at(actorType);
        // TODO : DB + 데이터로부터 로드해야합니다.
        const uint32 count = componentTypeList.size();
        for (uint32 index = 0; index < count; ++index)
        {
            switch (componentTypeList[index])
            {																										 
			case ActorComponentType::Move:																									 
				{							
                    CommonMoveComponentData data;
                    data._position = Vector(-2000, 900, 0);
                    data._rotation = Vector(0, 0, 0);
                    data._speed = 500.0f;
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
                    CommonAiComponentData data;
                    data._aiClassType = AiClassType::Player;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }

				}																															 
				break;																																	 
			case ActorComponentType::Character:
				{				
                    CommonCharacterComponentData data;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
				}																															 
				break;																																 
			case ActorComponentType::Inventory:
				{			
                    CommonInventoryComponentData data;

                    // 임시
                    {
                        data._itemSetType = ItemSetType::ContainerType;
                        data._capacity = 10;

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
            default:
                {
                    TA_COMPILE_DEV((5 == static_cast<uint8>(ActorComponentType::Count)), "여기도 추가해주세요")
                }
                break;
            }

        }

        return true;
    }

    bool ServerActorManager::initializeActorComponentsFromData(const ActorKey& targetActorKey, const CommonActorDetailSpawnData& detailSpawnData) noexcept
    {
        ServerActor* serverActor = static_cast<ServerActor*>(getActor(targetActorKey));
        if (nullptr == serverActor)
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }

        if (false == detailSpawnData._groupGameDataKey.isValid())
        {
            TA_ASSERT_DEV(false, "비정상입니다.");
            return false;
        }

        const GroupGameData* groupGameData = GetGameData<GroupGameData>(detailSpawnData._groupGameDataKey);
        if (nullptr == groupGameData)
        {
            TA_ASSERT_DEV(false, "데이터가 없습니다. GroupGameDataKey : %d", detailSpawnData._groupGameDataKey.getKeyValue());
            return false;
        }

        ActorType actorType = ActorType::Count;
        {
            ScopedLock actorLock(serverActor, true);
            actorType = serverActor->getActorType_();
        }

        const std::vector<ActorComponentType>& componentTypeList = ActorComponentGroups.at(actorType);

        const uint32 count = componentTypeList.size();
        for (uint32 index = 0; index < count; ++index)
        {
            switch (componentTypeList[index])
            {
            case ActorComponentType::Move:
                {
                    const MoveGameData* moveGameData = GetGameData<MoveGameData>(groupGameData->_moveGameDataKey);
                    if (nullptr == moveGameData)
                    {
                        TA_ASSERT_DEV(false, "데이터가 없습니다. MoveGameDataKey : %d", groupGameData->_moveGameDataKey.getKeyValue());
                        return false;
                    }
                   
                    CommonMoveComponentData data;
                    data._position = detailSpawnData._position;
                    data._rotation = detailSpawnData._rotation;
                    data._speed = moveGameData->_speed;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Action:
                {
                    //const ActionGameData* actionGameData = GetGameData<ActionGameData>(groupGameData->_actionGameDataKey);
                    //if (nullptr == actionGameData)
                    //{
                    //    TA_ASSERT_DEV(false, "데이터가 없습니다. ActionGameDataKey : %d", groupGameData->_actionGameDataKey.getKeyValue());
                    //    return false;
                    //}

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
                    const CharacterGameData* characterGameData = GetGameData<CharacterGameData>(groupGameData->_characterGameDataKey);
                    if (nullptr == characterGameData)
                    {
                        TA_ASSERT_DEV(false, "데이터가 없습니다. CharacterGameDataKey : %d", groupGameData->_characterGameDataKey.getKeyValue());
                        return false;
                    }

                    const AiGameData* aiGameData = GetGameData<AiGameData>(characterGameData->_aiGameDataKey);
                    if (nullptr == aiGameData)
                    {
                        TA_ASSERT_DEV(false, "데이터가 없습니다. AiGameDataKey : %d", characterGameData->_aiGameDataKey.getKeyValue());
                        return false;
                    }
                   
                    CommonAiComponentData data;
                    data._aiClassType = aiGameData->_aiClassType;
                    data._pathPointPathKey = aiGameData->_pathPointPathKey;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }

                }
                break;
            case ActorComponentType::Character:
                {
                    const CharacterGameData* characterGameData = GetGameData<CharacterGameData>(groupGameData->_characterGameDataKey);
                    if (nullptr == characterGameData)
                    {
                        TA_ASSERT_DEV(false, "데이터가 없습니다. CharacterGameDataKey : %d", groupGameData->_characterGameDataKey.getKeyValue());
                        return false;
                    }

                    CommonCharacterComponentData data;
                    if (false == initializeActorComponent(targetActorKey, &data, false))
                    {
                        TA_ASSERT_DEV(false, "비정상입니다.");
                        return false;
                    }
                }
                break;
            case ActorComponentType::Inventory:
                {
                    // 인벤토리 컨텐츠를 가진것들은 DB로 부터 초기화 되어야할것 같은데..
                    TA_ASSERT_DEV(false, "비정상입니다.");

                    //CommonInventoryComponentData data;
                    //if (false == initializeActorComponent(targetActorKey, &data, false))
                    //{
                    //    TA_ASSERT_DEV(false, "비정상입니다.");
                    //    return false;
                    //}
                }
                break;
            default:
                {
                    TA_COMPILE_DEV((5 == static_cast<uint8>(ActorComponentType::Count)), "여기도 추가해주세요")
                }
                break;
            }

        }

        return true;
    }


}

