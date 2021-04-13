#pragma once
#include "Common/AllPacketCommon.h"						  
#include "Server/ServerApp.h"						  
#include "Server/ServerActorManager.h"						  
#include "Common/CommonActor.h"						  
#include "Common/Socket.h" 

namespace ta
{
	static void SendCreateMainActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& actorKeyToCreate)
	{
		CreateMainActorSTC message
( actorKeyToCreate);
		message._length = sizeof(CreateMainActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendCreateActorSTC
( const ActorKey& networkActorKey,
 const uint32& actorType,
 const ActorKey& actorKeyToCreate)
	{
		CreateActorSTC message
( actorType,
 actorKeyToCreate);
		message._length = sizeof(CreateActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendDestroyActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& actorToDestroy)
	{
		DestroyActorSTC message
( actorToDestroy);
		message._length = sizeof(DestroyActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendMoveComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonMoveComponentData& componentData)
	{
		MoveComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(MoveComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendActionComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonActionComponentData& componentData)
	{
		ActionComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(ActionComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendAiComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonAiComponentData& componentData)
	{
		AiComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(AiComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendCharacterComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonCharacterComponentData& componentData)
	{
		CharacterComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(CharacterComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendInventoryComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonInventoryComponentData& componentData)
	{
		InventoryComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(InventoryComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendObjectComponentDataSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const CommonObjectComponentData& componentData)
	{
		ObjectComponentDataSTC message
( targetActorKey,
 componentData);
		message._length = sizeof(ObjectComponentDataSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendMoveActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const Vector& position)
	{
		MoveActorSTC message
( targetActorKey,
 position);
		message._length = sizeof(MoveActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendChangeHpSTC
( const ActorKey& networkActorKey,
 const ActorKey& attacker,
 const ActorKey& targetActorKey,
 const float& hpValue)
	{
		ChangeHpSTC message
( attacker,
 targetActorKey,
 hpValue);
		message._length = sizeof(ChangeHpSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendKillActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey)
	{
		KillActorSTC message
( targetActorKey);
		message._length = sizeof(KillActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendSetTargetActorSTC
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey)
	{
		SetTargetActorSTC message
( myActorKey,
 targetActorKey);
		message._length = sizeof(SetTargetActorSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendDropItemSTC
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount)
	{
		DropItemSTC message
( slotNo,
 stackCount);
		message._length = sizeof(DropItemSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


namespace ta
{
	static void SendUseItemSTC
( const ActorKey& networkActorKey,
 const ItemSlotNo& slotNo,
 const int32& stackCount)
	{
		UseItemSTC message
( slotNo,
 stackCount);
		message._length = sizeof(UseItemSTC);
		message._networkActorKey = networkActorKey;
		CommonActor* target = GetActorManager()->getActor(networkActorKey);
		if(nullptr == target)			
		{
			return;
		}
		Socket* network = target->getNetwork_();
		if(nullptr == network)			
		{
			return;
		}
		network->sendOverlapped(&message);
	}
}


