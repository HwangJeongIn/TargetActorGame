#pragma once
#include <functional>
#include "Common/CommonTypeDefinition.h"




namespace ta
{
	enum class TADataType : uint8
	{
		Int8 = 0
		, Int16
		, Int32
		, Int64
		, Uint8
		, Uint16
		, Uint32
		, Uint64
		, Bool
		, Float
		, Count
	};
}


namespace ta
{
	enum class ActorType : uint8
	{
		Default = 0
		, Owner
		, Player
		, ControlledByPlayer
		, Monster
		, Npc
		, Count
	};
}


namespace ta
{
	enum class ActorComponentType : uint8
	{
		Move = 0
		, Action
		, Ai
		, Character
		, Inventory
		, Count
	};
}


// 위 컴포넌트와 1:1 매칭이 안될 수 있음 // 한시스템이 여러개의 컴포넌트를 가지고 업데이트를 할 수 있다.
namespace ta
{
	enum class ActorSystemType : uint8
	{
		MoveSystem = 0
		, ActionSystem
		, AiSystem
		, InventorySystem
		, Count
	};
}


namespace ta
{
	enum class GameDataType : uint8
	{
		GroupGameData = 0
		, MoveGameData
		, AiGameData
		, CharacterGameData
		, ItemGameData
		, Count
	};
}


namespace ta
{
	enum class GameWorldType : uint8
	{
		RealWorld = 0
		, TempWorld
		, Count
	};
}


namespace ta
{
	constexpr uint32 MaxActorDataPoolCapacity = 100; //16384;//65536;
	constexpr uint32 MaxPacketBufferLength = 8192;
	constexpr uint32 MaxStringPackBufferLength = 256;
	constexpr uint32 MaxStringPathBufferLength = 512;
	constexpr uint32 MaxAsyncBufferCount = 64;
}


namespace ta
{
	enum class NetworkOwnerType : uint8
	{
		None = 0 // 일반 몬스터, NPC
		, Mine // 클라이언트레벨에서 서버와 연결될 액터 생성할때 / 서버레벨에서 클라이언트 받을때 
		, BorrowingNetwork // 클라이언트레벨에서 네트워크 공유할 액터 생성할때(계정1개에 여러개의 캐릭터) // 무조건 클라에서만 사용
	};
}


namespace ta
{
	enum class SocketType : uint8
	{
		Tcp = 0
		, Udp
	};
}


namespace ta
{
	enum class ThreadWorkType : uint8
	{
		SocketAccept = 0
		, SocketConnect
		, SocketSend
		, SocketReceive
		, SocketEventEnd
		//, SocketFirstReceiveForClient // 처음 액터키 받을때 컴플리션 키 변경하기 위해서
		, ActorEvent
		, ThreadEnd
		, Count
	};
}


namespace ta
{
	enum class PoolValueState : uint8
	{
		Waiting = 0 // 꺼내서 쓸 수 있음
		//, Reserving // 아직 초기화는 안되어있지만, 현재 예약중 ex) accept
		, Initializing // 사용전 초기화 단계 컴포넌트 초기화 등
		, Using // 사용중
		, Count
	};
}


namespace ta
{
	enum class  OriginDestinationType : uint8
	{
		STC = 0
		, CTS
	};

	enum class BracketType : uint8
	{
		Curly = 0
		, Round
		, Square
		, None
	};
}


namespace ta
{
	enum class ActorEventType : uint8
	{
		AiEvent = 0
		, LogTest
		, ThreadEnd
		, Count
	};

	enum class AiCommandType : uint8
	{
		TickAi = 0 // 현재 진행중인 Ai 수행
		, Move
		, Attack
		, Count
	};
}


namespace ta
{
	constexpr float MaxMoveDistance = 10.0f;
	constexpr long long AiTickInterval = 500;
}


namespace ta
{
	enum class AiState : uint8
	{
		Disabled = 0
		, None 
		, ChasingTarget
		, Attacking
		, Count
	};

	enum class AiClassType : uint8
	{
		Player = 0
		, Monster
		, Npc
		, Count
	};
}


namespace ta
{
	enum class ItemSetType : uint8
	{
		ContainerType = 0
		, SlotType
		, Count
	};

	enum class ItemType : uint8
	{
		All = 0 
		, Consumable
		, RHand
		, LHand
		, Count
	};

	typedef int		ItemSlotNo;
	constexpr uint8 MaxInventoryCapacity = 30;
}