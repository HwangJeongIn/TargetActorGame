#include "Client/AllPacketClient.h"
#include "Client/ClientApp.h"
#include "Client/ClientActorManager.h"
#include "Client/ClientMoveActorSystem.h"
#include "Client/ClientActionActorSystem.h"
#include "Client/ClientAiActorSystem.h"
#include "Client/ClientInventoryActorSystem.h"
#include "Client/ClientAiActorComponent.h"
#include "Common/AllPacketCommon.h"
#include "Common/CommonActor.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonInventoryActorComponent.h"



namespace ta
{
	void LoginCTS::processLoginCTS(const ActorKey& networkActorKey,
								   const StringPack& id,
								   const StringPack& password) noexcept
	{
		__noop;
	}

	void CreateMainActorSTC::processCreateMainActorSTC(const ActorKey& networkActorKey,
													   const ActorKey& actorKeyToCreate) noexcept
	{
		static_cast<ClientApp*>(g_app)->finishConnect(actorKeyToCreate);
	}

	void CreateActorSTC::processCreateActorSTC(const ActorKey& networkActorKey,
											   const uint32& actorType,
											   const ActorKey& actorKeyToCreate) noexcept
	{
		CommonActor* newActor = GetActorManager()->createActorForClient(static_cast<ActorType>(actorType), actorKeyToCreate);
		if (nullptr == newActor)
		{
			TA_ASSERT_DEV(false, "제대로 생성되지 않았습니다.")
		}
	}

	void DestroyActorSTC::processDestroyActorSTC(const ActorKey& networkActorKey,
												 const ActorKey& actorToDestroy) noexcept
	{
		if (false == GetActorManager()->destroyActor(actorToDestroy))
		{
			TA_ASSERT_DEV(false, "비정상입니다.")
		}
	}

	void MoveComponentDataSTC::processMoveComponentDataSTC(const ActorKey& networkActorKey,
														   const ActorKey& targetActorKey,
														   const CommonMoveComponentData& componentData) noexcept
	{
		TA_LOG_DEV("actor<%d> move to (%.1f, %.1f, %.1f) by component data / observer : actor<%d>"
				   , targetActorKey.getKeyValue()
				   , componentData._position._x
				   , componentData._position._y
				   , componentData._position._z
				   , networkActorKey.getKeyValue());

		GetActorManager()->initializeActorComponent(targetActorKey, &componentData, false);
	}

	void ActionComponentDataSTC::processActionComponentDataSTC(const ActorKey& networkActorKey,
															   const ActorKey& targetActorKey,
															   const CommonActionComponentData& componentData) noexcept
	{
		GetActorManager()->initializeActorComponent(targetActorKey, &componentData, false);
	}

	void AiComponentDataSTC::processAiComponentDataSTC(const ActorKey& networkActorKey,
													   const ActorKey& targetActorKey,
													   const CommonAiComponentData& componentData) noexcept
	{
		GetActorManager()->initializeActorComponent(targetActorKey, &componentData, false);
	}

	void CharacterComponentDataSTC::processCharacterComponentDataSTC(const ActorKey& networkActorKey,
																	 const ActorKey& targetActorKey,
																	 const CommonCharacterComponentData& componentData) noexcept
	{
		GetActorManager()->initializeActorComponent(targetActorKey, &componentData, false);
	}

	void InventoryComponentDataSTC::processInventoryComponentDataSTC(const ActorKey& networkActorKey,
																	 const ActorKey& targetActorKey,
																	 const CommonInventoryComponentData& componentData) noexcept
	{
		GetActorManager()->initializeActorComponent(targetActorKey, &componentData, false);
	}

	void MoveActorSTC::processMoveActorSTC(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey,
										   const Vector& position) noexcept
	{
		CommonActor* targetActor = GetActorManager()->getActor(targetActorKey);
		if (nullptr == targetActor)
		{
			return;
		}

		ClientMoveActorSystem* actorMoveSystem = GetActorSystem<ClientMoveActorSystem>();
		if (false == actorMoveSystem->respondMoveActor(targetActor, position))
		{
			return;
		}
	}

	//void RotateActorSTC::processRotateActorSTC(const ActorKey& networkActorKey,
	//										   const ActorKey& targetActorKey,
	//										   const Vector& direction) noexcept
	//{
	//	CommonMoveActorComponent* targetMove = GetActorComponent<CommonMoveActorComponent>(targetActorKey);
	//	if (nullptr == targetMove)
	//	{
	//		TA_ASSERT_DEV(false, "비정상입니다.");
	//		return;
	//	}
	//	
	//	ClientMoveActorSystem* actorMoveSystem = GetActorSystem<ClientMoveActorSystem>();
	//	if (false == actorMoveSystem->respondRotateActor(targetMove, direction))
	//	{
	//		TA_ASSERT_DEV(false, "비정상입니다.");
	//		return;
	//	}
	//}

	void MoveActorCTS::processMoveActorCTS(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey,
										   const Vector& position) noexcept
	{
		__noop;
	}

	void ChangeHpSTC::processChangeHpSTC(const ActorKey& networkActorKey,
										 const ActorKey& attacker,
										 const ActorKey& targetActorKey,
										 const uint32& hpValue) noexcept
	{
		ClientActionActorSystem* actorSystem = GetActorSystem<ClientActionActorSystem>();
		if (false == actorSystem->respondChangeHp(attacker, targetActorKey, hpValue))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void KillActorSTC::processKillActorSTC(const ActorKey& networkActorKey,
										   const ActorKey& targetActorKey) noexcept
	{
		ClientActionActorSystem* actorSystem = GetActorSystem<ClientActionActorSystem>();
		if (false == actorSystem->respondKillActor(targetActorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
		}
	}

	void AttackTargetCTS::processAttackTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const ActorKey& targetActorKey) noexcept
	{
		__noop;
	}

	void DetectTargetCTS::processDetectTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const uint32& targetActorType) noexcept
	{
		__noop;
	}

	void MoveToTargetCTS::processMoveToTargetCTS(const ActorKey& networkActorKey,
												 const ActorKey& myActorKey,
												 const ActorKey& targetActorKey) noexcept
	{
		__noop;
	}

	void MoveToRandomPointCTS::processMoveToRandomPointCTS(const ActorKey& networkActorKey,
														   const ActorKey& myActorKey) noexcept
	{
		__noop;
	}

	void SetTargetActorSTC::processSetTargetActorSTC(const ActorKey& networkActorKey,
													 const ActorKey& myActorKey,
													 const ActorKey& targetActorKey) noexcept
	{
		ClientAiActorComponent* myAi = GetActorComponent<ClientAiActorComponent>(myActorKey);
		if (nullptr == myAi)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		ClientAiActorSystem* aiSystem = GetActorSystem<ClientAiActorSystem>();
		if (false == aiSystem->setTargetActor(myAi, targetActorKey))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}
	}

	void DropItemCTS::processDropItemCTS(const ActorKey& networkActorKey,
										 const ItemSlotNo& slotNo,
										 const int32& stackCount) noexcept
	{
		__noop;
	}

	void UseItemCTS::processUseItemCTS(const ActorKey& networkActorKey,
									   const ItemSlotNo& slotNo,
									   const int32& stackCount) noexcept
	{
		__noop;
	}

	void DropItemSTC::processDropItemSTC(const ActorKey& networkActorKey,
										 const ItemSlotNo& slotNo,
										 const int32& stackCount) noexcept
	{
		ClientInventoryActorSystem* clientInventorySystem = GetActorSystem<ClientInventoryActorSystem>();
		CommonInventoryActorComponent* inventoryCom = GetActorComponent<CommonInventoryActorComponent>(networkActorKey);
		if (nullptr == inventoryCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == clientInventorySystem->respondDropItem(inventoryCom, slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}
	}
	
	void UseItemSTC::processUseItemSTC(const ActorKey& networkActorKey,
									   const ItemSlotNo& slotNo,
									   const int32& stackCount) noexcept
	{
		ClientInventoryActorSystem* clientInventorySystem = GetActorSystem<ClientInventoryActorSystem>();
		CommonInventoryActorComponent* inventoryCom = GetActorComponent<CommonInventoryActorComponent>(networkActorKey);
		if (nullptr == inventoryCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		if (false == clientInventorySystem->respondUseItem(inventoryCom, slotNo, stackCount))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}
	}
}