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
 ObjectComponentDataSTC,
 MoveActorSTC,
 ChangeHpSTC,
 KillActorSTC,
 SetTargetActorSTC,
 DropItemSTC,
 UseItemSTC,
 DoBuffSTC,
 UndoBuffSTC,
 LoginCTS,
 MoveActorCTS,
 AttackTargetCTS,
 DetectTargetCTS,
 MoveToTargetCTS,
 MoveToRandomPointCTS,
 DropItemCTS,
 UseItemCTS};
}


