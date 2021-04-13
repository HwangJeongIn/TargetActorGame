#pragma once

#include "Common/ComponentData.h"

namespace ta
{
	// 여기 일단 사용안해서 그냥 갱신만 해줬음
	TA_COMPILE_DEV(6 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
}

namespace ta
{
	class ClientMoveComponentData : public CommonMoveComponentData
	{
	public:
		explicit ClientMoveComponentData(void) noexcept;
		virtual ~ClientMoveComponentData(void) noexcept;

	public:
	};
}


namespace ta
{
	class ClientActionComponentData : public CommonActionComponentData
	{
	public:
		explicit ClientActionComponentData(void) noexcept;
		virtual ~ClientActionComponentData(void) noexcept;

	public:
	};
}


namespace ta
{
	class ClientAiComponentData : public CommonAiComponentData
	{
	public:
		explicit ClientAiComponentData(void) noexcept;
		virtual ~ClientAiComponentData(void) noexcept;

	public:
	};
}