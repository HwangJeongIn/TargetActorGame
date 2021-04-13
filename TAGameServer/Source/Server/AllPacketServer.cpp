#include "Common/AllPacketCommon.h"
#include "Server/AllPacketServer.h"
#include "Common/GetComponentAndSystem.h"
#include "Server/ServerMoveActorSystem.h"
#include "Server/ServerActionActorSystem.h"
#include "Server/ServerAiActorSystem.h"
#include "Server/ServerInventoryActorSystem.h"
#include "Server/ServerActor.h"
#include "Common/CommonAiActorComponent.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonInventoryActorComponent.h"
#include "Common/ScopedLock.h"



namespace ta
{
	void LoginCTS::processLoginCTS(const ActorKey& networkActorKey
								   , const StringPack& id
								   , const StringPack& password) noexcept
	{
		TA_LOG_DEV("id : %s, password : %s", id.getAsTstring().data(), password.getAsTstring().data());
		// 아이디 비밀번호 검증 > 로그인 성공 > DB fetch > MainActorCreatedSTC 패킷(클라에서 액터만 생성) > fetch받은걸로 클라에게 컴포넌트 초기화 패킷도 내려준다. 
		// 일단 받았다 치자. // 사실 그냥 기본값으로 다 초기화되어있음
		ServerActor* serverActor = static_cast<ServerActor*>(GetActorManager()->getActor(networkActorKey));
		if (nullptr == serverActor)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		// TODO : DB에서 받아온 캐릭터 번호
		CharacterDBNo characterDBNo(1);
		if (false == GetActorManager()->initializeActorComponentsFromDB(serverActor->getActorKey(), characterDBNo))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		SendCreateMainActorSTC(networkActorKey, networkActorKey); // 일단 서버에서는 액터키로 생성된 상태
		// 클라의 컴포넌트들을 클라본인에게 알려준다.
		SendAllComponentDataToClient(networkActorKey, networkActorKey);
	}

	void CreateMainActorSTC::processCreateMainActorSTC(const ActorKey& networkActorKey,
													   const ActorKey& actorKeyToCreate) noexcept
	{
		__noop;
	}

	void CreateActorSTC::processCreateActorSTC(const ActorKey& networkActorKey,
											   const uint32& actorType,
											   const ActorKey& actorKeyToCreate) noexcept
	{
		__noop;
	}

	void DestroyActorSTC::processDestroyActorSTC(const ActorKey& networkActorKey,
												 const ActorKey& actorToDestroy) noexcept
	{
		__noop;
	}

	void MoveComponentDataSTC::processMoveComponentDataSTC(const ActorKey& networkActorKey,
														   const ActorKey& targetActorKey,
														   const CommonMoveComponentData& componentData) noexcept
	{
		__noop;
	}

	void ActionComponentDataSTC::processActionComponentDataSTC(const ActorKey& networkActorKey,
															   const ActorKey& targetActorKey,
															   const CommonActionComponentData& componentData) noexcept
	{
		__noop;
	}

	void AiComponentDataSTC::processAiComponentDataSTC(const ActorKey& networkActorKey,
													   const ActorKey& targetActorKey,
													   const CommonAiComponentData& componentData) noexcept
	{
		__noop;
	}

	void CharacterComponentDataSTC::processCharacterComponentDataSTC(const ActorKey& networkActorKey,
																	 const ActorKey& targetActorKey,
																	 const CommonCharacterComponentData& componentData) noexcept
	{
		__noop;
	}

	void InventoryComponentDataSTC::processInventoryComponentDataSTC(const ActorKey& networkActorKey,
																	 const ActorKey& targetActorKey,
																	 const CommonInventoryComponentData& componentData) noexcept
	{
		__noop;
	}

	void ObjectComponentDataSTC::processObjectComponentDataSTC(const ActorKey& networkActorKey,
															   const ActorKey& targetActorKey,
															   const CommonObjectComponentData& componentData) noexcept
	{
		__noop;
	}

	void MoveActorSTC::processMoveActorSTC(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey,
										   const Vector& position) noexcept
	{
		__noop;
	}

	void MoveActorCTS::processMoveActorCTS(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey,
										   const Vector& position) noexcept
	{
		CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
		if (nullptr == targetActor)
		{
			return;
		}
		
		ServerMoveActorSystem* actorMoveSystem = GetActorSystem<ServerMoveActorSystem>();
		if (false == actorMoveSystem->respondMoveActor(targetActor, position))
		{
			return;
		}
	}

	void ChangeHpSTC::processChangeHpSTC(const ActorKey& networkActorKey,
										 const ActorKey& attacker,
										 const ActorKey& targetActorKey,
										 const float& hpValue) noexcept
	{
		__noop;
	}

	void KillActorSTC::processKillActorSTC(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey) noexcept
	{
		__noop;
	}

	void AttackTargetCTS::processAttackTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const ActorKey& targetActorKey) noexcept
	{
		ServerActionActorSystem* serverActionSystem = GetActorSystem<ServerActionActorSystem>();
		if (false == serverActionSystem->respondAttackTarget(myActorKey, targetActorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void DetectTargetCTS::processDetectTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const uint32& targetActorType) noexcept
	{
		ServerAiActorSystem* serverAiSystem = GetActorSystem<ServerAiActorSystem>();
		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(myActorKey);
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(myActorKey);
		if (nullptr == moveCom
			|| nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == serverAiSystem->detectTarget(aiCom, moveCom, static_cast<ActorType>(targetActorType)))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void MoveToTargetCTS::processMoveToTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const ActorKey& targetActorKey) noexcept
	{
		ServerMoveActorSystem* serverMoveSystem = GetActorSystem<ServerMoveActorSystem>();
		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(myActorKey);
		CommonMoveActorComponent* targetMoveCom = GetActorComponent<CommonMoveActorComponent>(targetActorKey);
		if (nullptr == moveCom
			|| nullptr == targetMoveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == serverMoveSystem->moveToTarget(moveCom, targetMoveCom))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void MoveToRandomPointCTS::processMoveToRandomPointCTS(const ActorKey& networkActorKey,
														   const ActorKey& myActorKey) noexcept
	{
		ServerMoveActorSystem* serverMoveSystem = GetActorSystem<ServerMoveActorSystem>();
		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(myActorKey);
		if (nullptr == moveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == serverMoveSystem->moveToRandomPoint(moveCom))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void SetTargetActorSTC::processSetTargetActorSTC(const ActorKey& networkActorKey,
													 const ActorKey& myActorKey,
													 const ActorKey& targetActorKey) noexcept
	{
		__noop;
	}

	void DropItemCTS::processDropItemCTS(const ActorKey& networkActorKey,
										 const ItemSlotNo& slotNo,
										 const int32& stackCount) noexcept
	{
		ServerInventoryActorSystem* serverInventorySystem = GetActorSystem<ServerInventoryActorSystem>();
		CommonInventoryActorComponent* inventoryCom = GetActorComponent<CommonInventoryActorComponent>(networkActorKey);
		if (nullptr == inventoryCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == serverInventorySystem->respondDropItem(inventoryCom, slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}
	}

	void UseItemCTS::processUseItemCTS(const ActorKey& networkActorKey,
									   const ItemSlotNo& slotNo,
									   const int32& stackCount) noexcept
	{
		ServerInventoryActorSystem* serverInventorySystem = GetActorSystem<ServerInventoryActorSystem>();
		CommonInventoryActorComponent* inventoryCom = GetActorComponent<CommonInventoryActorComponent>(networkActorKey);
		if (nullptr == inventoryCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == serverInventorySystem->respondUseItem(inventoryCom, slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}
	}

	void DropItemSTC::processDropItemSTC(const ActorKey& networkActorKey,
										 const ItemSlotNo& slotNo,
										 const int32& stackCount) noexcept
	{
		__noop;
	}

	void UseItemSTC::processUseItemSTC(const ActorKey& networkActorKey,
									   const ItemSlotNo& slotNo,
									   const int32& stackCount) noexcept
	{
		__noop;
	}
}