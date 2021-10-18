﻿#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include "Common/KeyDefinition.h"
#include "Common/ItemDetail.h"
#include <vector>


namespace ta
{
	TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
}

// 패킷에도 사용되는 자료구조
// Common인 경우 클라정보 서버정보 구분이 필요한경우 양측데이터를 모두 가지고 있도록 허용한다.

namespace ta
{
//#pragma pack(push, 1)
	class ComponentData
	{
	public:
		explicit ComponentData(const ActorComponentType& type) noexcept;
		virtual ~ComponentData(void) noexcept;

		const ActorComponentType getActorComponentType(void) const noexcept;

	private:
		ActorComponentType _type;
	};
}


namespace ta
{
//#pragma pack(push, 1)
	class CommonMoveComponentData : public ComponentData
	{
	public:
		explicit CommonMoveComponentData(void) noexcept;
		virtual ~CommonMoveComponentData(void) noexcept;

		void clear(void) noexcept;
	public:
		Vector _position;
		Vector _rotation;
		float _speed;
	};
}


namespace ta
{
//#pragma pack(push, 1)
	class CommonActionComponentData : public ComponentData
	{
	public:
		explicit CommonActionComponentData(void) noexcept;
		virtual ~CommonActionComponentData(void) noexcept;

		void clear(void) noexcept;
	public:
	};
}


namespace ta
{
//#pragma pack(push, 1)
	class CommonAiComponentData : public ComponentData
	{
	public:
		explicit CommonAiComponentData(void) noexcept;
		virtual ~CommonAiComponentData(void) noexcept;

		void clear(void) noexcept;
	public:
		AiClassType _aiClassType;
		PathPointPathKey _pathPointPathKey;
	};
}


namespace ta
{
//#pragma pack(push, 1)
	class CommonCharacterComponentData : public ComponentData
	{
	public:
		explicit CommonCharacterComponentData(void) noexcept;
		virtual ~CommonCharacterComponentData(void) noexcept;

		void clear(void) noexcept;
	public:
		CharacterGameDataKey _characterGameDataKey;
		float _currentStats[static_cast<uint8>(CharacterStatType::Count)];
	};
}


namespace ta
{
	class ItemElementData
	{
	public:
		ItemElementData(void) noexcept;
		virtual ~ItemElementData(void) noexcept;

		void clear(void) noexcept;

		ItemGameDataKey _baseKey;
		ItemDetail _detail;
		int32 _stackCount;
	};
}


namespace ta
{
	//#pragma pack(push, 1)
	class CommonInventoryComponentData : public ComponentData
	{
	public:
		explicit CommonInventoryComponentData(void) noexcept;
		virtual ~CommonInventoryComponentData(void) noexcept;

		void clear(void) noexcept;

	public:
		ItemSetType _itemSetType;
		int32 _capacity;
		ItemElementData _itemElementDataSet[MaxInventoryCapacity]; // 벡터 그대로 넘기면 안된다 구조 바뀌면 벡터에서 하나씩 빼서 serialize할 예정


	};
}


namespace ta
{
//#pragma pack(push, 1)
	class CommonObjectComponentData : public ComponentData
	{
	public:
		explicit CommonObjectComponentData(void) noexcept;
		virtual ~CommonObjectComponentData(void) noexcept;

		void clear(void) noexcept;
	public:
		ItemGameDataKey _itemGameDataKey;
		RenderingGameDataKey _renderingGameDataKey;
	};
}