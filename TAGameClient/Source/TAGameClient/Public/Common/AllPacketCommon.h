#pragma once
#include "Common/MessagePack.h"

namespace ta
{
	class CreateMainActorSTC : public MessagePack
	{
	public:
		 explicit CreateMainActorSTC
( const ActorKey& actorKeyToCreate) noexcept
			: MessagePack(MessageType::CreateMainActorSTC),
 		 actorKeyToCreate(actorKeyToCreate)  {}
		virtual ~CreateMainActorSTC(void) noexcept {}
		static void processMessage(const CreateMainActorSTC* message) noexcept 
{ 
CreateMainActorSTC::processCreateMainActorSTC
( message->_networkActorKey,
 message->actorKeyToCreate);
}
		static void processCreateMainActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& actorKeyToCreate) noexcept;

	public:
 	 ActorKey actorKeyToCreate; 
	};
}


namespace ta
{
	class CreateActorSTC : public MessagePack
	{
	public:
		 explicit CreateActorSTC
( const uint32& actorType,
 const ActorKey& actorKeyToCreate) noexcept
			: MessagePack(MessageType::CreateActorSTC),
 		 actorType(actorType),
		 actorKeyToCreate(actorKeyToCreate)  {}
		virtual ~CreateActorSTC(void) noexcept {}
		static void processMessage(const CreateActorSTC* message) noexcept 
{ 
CreateActorSTC::processCreateActorSTC
( message->_networkActorKey,
 message->actorType,
 message->actorKeyToCreate);
}
		static void processCreateActorSTC
( const ActorKey& networkActorKey,
 const uint32& actorType,
 const ActorKey& actorKeyToCreate) noexcept;

	public:
 	 uint32 actorType;
	 ActorKey actorKeyToCreate; 
	};
}


namespace ta
{
	class DestroyActorSTC : public MessagePack
	{
	public:
		 explicit DestroyActorSTC
( const ActorKey& actorToDestroy) noexcept
			: MessagePack(MessageType::DestroyActorSTC),
 		 actorToDestroy(actorToDestroy)  {}
		virtual ~DestroyActorSTC(void) noexcept {}
		static void processMessage(const DestroyActorSTC* message) noexcept 
{ 
DestroyActorSTC::processDestroyActorSTC
( message->_networkActorKey,
 message->actorToDestroy);
}
		static void processDestroyActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& actorToDestroy) noexcept;

	public:
 	 ActorKey actorToDestroy; 
	};
}


namespace ta
{
	class MoveComponentDataSTC : public MessagePack
	{
	public:
		 explicit MoveComponentDataSTC
( const ActorKey& targetActorKey,
 const CommonMoveComponentData& componentData) noexcept
			: MessagePack(MessageType::MoveComponentDataSTC),
 		 targetActorKey(targetActorKey),
		 componentData(componentData)  {}
		virtual ~MoveComponentDataSTC(void) noexcept {}
		static void processMessage(const MoveComponentDataSTC* message) noexcept 
{ 
MoveComponentDataSTC::processMoveComponentDataSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->componentData);
}
		static void processMoveComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonMoveComponentData& componentData) noexcept;

	public:
 	 ActorKey targetActorKey;
	 CommonMoveComponentData componentData; 
	};
}


namespace ta
{
	class ActionComponentDataSTC : public MessagePack
	{
	public:
		 explicit ActionComponentDataSTC
( const ActorKey& targetActorKey,
 const CommonActionComponentData& componentData) noexcept
			: MessagePack(MessageType::ActionComponentDataSTC),
 		 targetActorKey(targetActorKey),
		 componentData(componentData)  {}
		virtual ~ActionComponentDataSTC(void) noexcept {}
		static void processMessage(const ActionComponentDataSTC* message) noexcept 
{ 
ActionComponentDataSTC::processActionComponentDataSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->componentData);
}
		static void processActionComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonActionComponentData& componentData) noexcept;

	public:
 	 ActorKey targetActorKey;
	 CommonActionComponentData componentData; 
	};
}


namespace ta
{
	class AiComponentDataSTC : public MessagePack
	{
	public:
		 explicit AiComponentDataSTC
( const ActorKey& targetActorKey,
 const CommonAiComponentData& componentData) noexcept
			: MessagePack(MessageType::AiComponentDataSTC),
 		 targetActorKey(targetActorKey),
		 componentData(componentData)  {}
		virtual ~AiComponentDataSTC(void) noexcept {}
		static void processMessage(const AiComponentDataSTC* message) noexcept 
{ 
AiComponentDataSTC::processAiComponentDataSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->componentData);
}
		static void processAiComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonAiComponentData& componentData) noexcept;

	public:
 	 ActorKey targetActorKey;
	 CommonAiComponentData componentData; 
	};
}


namespace ta
{
	class CharacterComponentDataSTC : public MessagePack
	{
	public:
		 explicit CharacterComponentDataSTC
( const ActorKey& targetActorKey,
 const CommonCharacterComponentData& componentData) noexcept
			: MessagePack(MessageType::CharacterComponentDataSTC),
 		 targetActorKey(targetActorKey),
		 componentData(componentData)  {}
		virtual ~CharacterComponentDataSTC(void) noexcept {}
		static void processMessage(const CharacterComponentDataSTC* message) noexcept 
{ 
CharacterComponentDataSTC::processCharacterComponentDataSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->componentData);
}
		static void processCharacterComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonCharacterComponentData& componentData) noexcept;

	public:
 	 ActorKey targetActorKey;
	 CommonCharacterComponentData componentData; 
	};
}


namespace ta
{
	class InventoryComponentDataSTC : public MessagePack
	{
	public:
		 explicit InventoryComponentDataSTC
( const ActorKey& targetActorKey,
 const CommonInventoryComponentData& componentData) noexcept
			: MessagePack(MessageType::InventoryComponentDataSTC),
 		 targetActorKey(targetActorKey),
		 componentData(componentData)  {}
		virtual ~InventoryComponentDataSTC(void) noexcept {}
		static void processMessage(const InventoryComponentDataSTC* message) noexcept 
{ 
InventoryComponentDataSTC::processInventoryComponentDataSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->componentData);
}
		static void processInventoryComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonInventoryComponentData& componentData) noexcept;

	public:
 	 ActorKey targetActorKey;
	 CommonInventoryComponentData componentData; 
	};
}


namespace ta
{
	class MoveActorSTC : public MessagePack
	{
	public:
		 explicit MoveActorSTC
( const ActorKey& targetActorKey,
 const Vector& position) noexcept
			: MessagePack(MessageType::MoveActorSTC),
 		 targetActorKey(targetActorKey),
		 position(position)  {}
		virtual ~MoveActorSTC(void) noexcept {}
		static void processMessage(const MoveActorSTC* message) noexcept 
{ 
MoveActorSTC::processMoveActorSTC
( message->_networkActorKey,
 message->targetActorKey,
 message->position);
}
		static void processMoveActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const Vector& position) noexcept;

	public:
 	 ActorKey targetActorKey;
	 Vector position; 
	};
}


namespace ta
{
	class ChangeHpSTC : public MessagePack
	{
	public:
		 explicit ChangeHpSTC
( const ActorKey& attacker,
 const ActorKey& targetActorKey,
 const uint32& hpValue) noexcept
			: MessagePack(MessageType::ChangeHpSTC),
 		 attacker(attacker),
		 targetActorKey(targetActorKey),
		 hpValue(hpValue)  {}
		virtual ~ChangeHpSTC(void) noexcept {}
		static void processMessage(const ChangeHpSTC* message) noexcept 
{ 
ChangeHpSTC::processChangeHpSTC
( message->_networkActorKey,
 message->attacker,
 message->targetActorKey,
 message->hpValue);
}
		static void processChangeHpSTC
( const ActorKey& networkActorKey,
 const ActorKey& attacker,
 const ActorKey& targetActorKey,
 const uint32& hpValue) noexcept;

	public:
 	 ActorKey attacker;
	 ActorKey targetActorKey;
	 uint32 hpValue; 
	};
}


namespace ta
{
	class KillActorSTC : public MessagePack
	{
	public:
		 explicit KillActorSTC
( const ActorKey& targetActorKey) noexcept
			: MessagePack(MessageType::KillActorSTC),
 		 targetActorKey(targetActorKey)  {}
		virtual ~KillActorSTC(void) noexcept {}
		static void processMessage(const KillActorSTC* message) noexcept 
{ 
KillActorSTC::processKillActorSTC
( message->_networkActorKey,
 message->targetActorKey);
}
		static void processKillActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey) noexcept;

	public:
 	 ActorKey targetActorKey; 
	};
}


namespace ta
{
	class SetTargetActorSTC : public MessagePack
	{
	public:
		 explicit SetTargetActorSTC
( const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept
			: MessagePack(MessageType::SetTargetActorSTC),
 		 myActorKey(myActorKey),
		 targetActorKey(targetActorKey)  {}
		virtual ~SetTargetActorSTC(void) noexcept {}
		static void processMessage(const SetTargetActorSTC* message) noexcept 
{ 
SetTargetActorSTC::processSetTargetActorSTC
( message->_networkActorKey,
 message->myActorKey,
 message->targetActorKey);
}
		static void processSetTargetActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept;

	public:
 	 ActorKey myActorKey;
	 ActorKey targetActorKey; 
	};
}


namespace ta
{
	class DropItemSTC : public MessagePack
	{
	public:
		 explicit DropItemSTC
( const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept
			: MessagePack(MessageType::DropItemSTC),
 		 slotNo(slotNo),
		 stackCount(stackCount)  {}
		virtual ~DropItemSTC(void) noexcept {}
		static void processMessage(const DropItemSTC* message) noexcept 
{ 
DropItemSTC::processDropItemSTC
( message->_networkActorKey,
 message->slotNo,
 message->stackCount);
}
		static void processDropItemSTC
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept;

	public:
 	 ItemSlotNo slotNo;
	 int32 stackCount; 
	};
}


namespace ta
{
	class UseItemSTC : public MessagePack
	{
	public:
		 explicit UseItemSTC
( const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept
			: MessagePack(MessageType::UseItemSTC),
 		 slotNo(slotNo),
		 stackCount(stackCount)  {}
		virtual ~UseItemSTC(void) noexcept {}
		static void processMessage(const UseItemSTC* message) noexcept 
{ 
UseItemSTC::processUseItemSTC
( message->_networkActorKey,
 message->slotNo,
 message->stackCount);
}
		static void processUseItemSTC
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept;

	public:
 	 ItemSlotNo slotNo;
	 int32 stackCount; 
	};
}


namespace ta
{
	class LoginCTS : public MessagePack
	{
	public:
		 explicit LoginCTS
( const StringPack& id,
 const StringPack& password) noexcept
			: MessagePack(MessageType::LoginCTS),
 		 id(id),
		 password(password)  {}
		virtual ~LoginCTS(void) noexcept {}
		static void processMessage(const LoginCTS* message) noexcept 
{ 
LoginCTS::processLoginCTS
( message->_networkActorKey,
 message->id,
 message->password);
}
		static void processLoginCTS
( const ActorKey& networkActorKey,
 const StringPack& id,
 const StringPack& password) noexcept;

	public:
 	 StringPack id;
	 StringPack password; 
	};
}


namespace ta
{
	class MoveActorCTS : public MessagePack
	{
	public:
		 explicit MoveActorCTS
( const ActorKey& targetActorKey,
 const Vector& position) noexcept
			: MessagePack(MessageType::MoveActorCTS),
 		 targetActorKey(targetActorKey),
		 position(position)  {}
		virtual ~MoveActorCTS(void) noexcept {}
		static void processMessage(const MoveActorCTS* message) noexcept 
{ 
MoveActorCTS::processMoveActorCTS
( message->_networkActorKey,
 message->targetActorKey,
 message->position);
}
		static void processMoveActorCTS
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const Vector& position) noexcept;

	public:
 	 ActorKey targetActorKey;
	 Vector position; 
	};
}


namespace ta
{
	class AttackTargetCTS : public MessagePack
	{
	public:
		 explicit AttackTargetCTS
( const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept
			: MessagePack(MessageType::AttackTargetCTS),
 		 myActorKey(myActorKey),
		 targetActorKey(targetActorKey)  {}
		virtual ~AttackTargetCTS(void) noexcept {}
		static void processMessage(const AttackTargetCTS* message) noexcept 
{ 
AttackTargetCTS::processAttackTargetCTS
( message->_networkActorKey,
 message->myActorKey,
 message->targetActorKey);
}
		static void processAttackTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept;

	public:
 	 ActorKey myActorKey;
	 ActorKey targetActorKey; 
	};
}


namespace ta
{
	class DetectTargetCTS : public MessagePack
	{
	public:
		 explicit DetectTargetCTS
( const ActorKey& myActorKey,
 const uint32& targetActorType) noexcept
			: MessagePack(MessageType::DetectTargetCTS),
 		 myActorKey(myActorKey),
		 targetActorType(targetActorType)  {}
		virtual ~DetectTargetCTS(void) noexcept {}
		static void processMessage(const DetectTargetCTS* message) noexcept 
{ 
DetectTargetCTS::processDetectTargetCTS
( message->_networkActorKey,
 message->myActorKey,
 message->targetActorType);
}
		static void processDetectTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const uint32& targetActorType) noexcept;

	public:
 	 ActorKey myActorKey;
	 uint32 targetActorType; 
	};
}


namespace ta
{
	class MoveToTargetCTS : public MessagePack
	{
	public:
		 explicit MoveToTargetCTS
( const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept
			: MessagePack(MessageType::MoveToTargetCTS),
 		 myActorKey(myActorKey),
		 targetActorKey(targetActorKey)  {}
		virtual ~MoveToTargetCTS(void) noexcept {}
		static void processMessage(const MoveToTargetCTS* message) noexcept 
{ 
MoveToTargetCTS::processMoveToTargetCTS
( message->_networkActorKey,
 message->myActorKey,
 message->targetActorKey);
}
		static void processMoveToTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey) noexcept;

	public:
 	 ActorKey myActorKey;
	 ActorKey targetActorKey; 
	};
}


namespace ta
{
	class MoveToRandomPointCTS : public MessagePack
	{
	public:
		 explicit MoveToRandomPointCTS
( const ActorKey& myActorKey) noexcept
			: MessagePack(MessageType::MoveToRandomPointCTS),
 		 myActorKey(myActorKey)  {}
		virtual ~MoveToRandomPointCTS(void) noexcept {}
		static void processMessage(const MoveToRandomPointCTS* message) noexcept 
{ 
MoveToRandomPointCTS::processMoveToRandomPointCTS
( message->_networkActorKey,
 message->myActorKey);
}
		static void processMoveToRandomPointCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey) noexcept;

	public:
 	 ActorKey myActorKey; 
	};
}


namespace ta
{
	class DropItemCTS : public MessagePack
	{
	public:
		 explicit DropItemCTS
( const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept
			: MessagePack(MessageType::DropItemCTS),
 		 slotNo(slotNo),
		 stackCount(stackCount)  {}
		virtual ~DropItemCTS(void) noexcept {}
		static void processMessage(const DropItemCTS* message) noexcept 
{ 
DropItemCTS::processDropItemCTS
( message->_networkActorKey,
 message->slotNo,
 message->stackCount);
}
		static void processDropItemCTS
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept;

	public:
 	 ItemSlotNo slotNo;
	 int32 stackCount; 
	};
}


namespace ta
{
	class UseItemCTS : public MessagePack
	{
	public:
		 explicit UseItemCTS
( const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept
			: MessagePack(MessageType::UseItemCTS),
 		 slotNo(slotNo),
		 stackCount(stackCount)  {}
		virtual ~UseItemCTS(void) noexcept {}
		static void processMessage(const UseItemCTS* message) noexcept 
{ 
UseItemCTS::processUseItemCTS
( message->_networkActorKey,
 message->slotNo,
 message->stackCount);
}
		static void processUseItemCTS
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount) noexcept;

	public:
 	 ItemSlotNo slotNo;
	 int32 stackCount; 
	};
}


