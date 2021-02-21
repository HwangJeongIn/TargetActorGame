#pragma once
#include "Common/AllPacketCommon.h"						  
#include "Client/ClientApp.h"						  
#include "Client/ClientActorManager.h"						  
#include "Common/CommonActor.h"						  
#include "Common/Socket.h" 

namespace ta
{
	static void SendLoginCTS
( const ActorKey& networkActorKey,
 const StringPack& id,
 const StringPack& password)
	{
		LoginCTS message
( id,
 password);
		message._length = sizeof(LoginCTS);
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
	static void SendMoveActorCTS
( const ActorKey& networkActorKey,
 const ActorKey& targetActorKey,
 const Vector& position)
	{
		MoveActorCTS message
( targetActorKey,
 position);
		message._length = sizeof(MoveActorCTS);
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
	static void SendAttackTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey)
	{
		AttackTargetCTS message
( myActorKey,
 targetActorKey);
		message._length = sizeof(AttackTargetCTS);
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
	static void SendDetectTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const uint32& targetActorType)
	{
		DetectTargetCTS message
( myActorKey,
 targetActorType);
		message._length = sizeof(DetectTargetCTS);
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
	static void SendMoveToTargetCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey,
 const ActorKey& targetActorKey)
	{
		MoveToTargetCTS message
( myActorKey,
 targetActorKey);
		message._length = sizeof(MoveToTargetCTS);
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
	static void SendMoveToRandomPointCTS
( const ActorKey& networkActorKey,
 const ActorKey& myActorKey)
	{
		MoveToRandomPointCTS message
( myActorKey);
		message._length = sizeof(MoveToRandomPointCTS);
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


