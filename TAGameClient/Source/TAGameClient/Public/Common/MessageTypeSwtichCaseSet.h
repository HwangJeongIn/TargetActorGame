#pragma once


#define MESSAGE_SWITCH_CASE(TypeName)\
							case MessageType::TypeName:\
							{\
								TypeName::processMessage(static_cast<const TypeName*>(message));\
							}\
							break;\

MESSAGE_SWITCH_CASE(CreateMainActorSTC)
MESSAGE_SWITCH_CASE(CreateActorSTC)
MESSAGE_SWITCH_CASE(DestroyActorSTC)
MESSAGE_SWITCH_CASE(MoveComponentDataSTC)
MESSAGE_SWITCH_CASE(ActionComponentDataSTC)
MESSAGE_SWITCH_CASE(AiComponentDataSTC)
MESSAGE_SWITCH_CASE(CharacterComponentDataSTC)
MESSAGE_SWITCH_CASE(InventoryComponentDataSTC)
MESSAGE_SWITCH_CASE(MoveActorSTC)
MESSAGE_SWITCH_CASE(ChangeHpSTC)
MESSAGE_SWITCH_CASE(KillActorSTC)
MESSAGE_SWITCH_CASE(SetTargetActorSTC)
MESSAGE_SWITCH_CASE(LoginCTS)
MESSAGE_SWITCH_CASE(MoveActorCTS)
MESSAGE_SWITCH_CASE(AttackTargetCTS)
MESSAGE_SWITCH_CASE(DetectTargetCTS)
MESSAGE_SWITCH_CASE(MoveToTargetCTS)
MESSAGE_SWITCH_CASE(MoveToRandomPointCTS)


#undef MESSAGE_SWITCH_CASE
