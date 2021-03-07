#pragma once
#include "Common/MessagePack.h"

namespace ta
{
enum class MessageType : uint32
{ CreateMainActorSTC,
 CreateActorSTC,
 DestroyActorSTC,
 MoveComponentDataSTC,
 ActionComponentDataSTC,
 AiComponentDataSTC,
 CharacterComponentDataSTC,
 InventoryComponentDataSTC,
 MoveActorSTC,
 ChangeHpSTC,
 KillActorSTC,
 SetTargetActorSTC,
 DropItemSTC,
 UseItemSTC,
 LoginCTS,
 MoveActorCTS,
 AttackTargetCTS,
 DetectTargetCTS,
 MoveToTargetCTS,
 MoveToRandomPointCTS,
 DropItemCTS,
 UseItemCTS};
}


