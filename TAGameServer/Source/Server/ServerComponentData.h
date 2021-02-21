#pragma once

#include "Common/ComponentData.h"


namespace ta
{
	// 여기 일단 사용안해서 그냥 갱신만 해줬음
	TA_COMPILE_DEV(5 == static_cast<uint8>(ActorComponentType::Count), "여기도 추가해주세요");
}

namespace ta
{
	class ServerMoveComponentData : public CommonMoveComponentData
	{
	public:
		explicit ServerMoveComponentData(void) noexcept;
		virtual ~ServerMoveComponentData(void) noexcept;

	public:
	};
}


namespace ta
{
	class ServerActionComponentData : public CommonActionComponentData
	{
	public:
		explicit ServerActionComponentData(void) noexcept;
		virtual ~ServerActionComponentData(void) noexcept;

	public:
	};
}


namespace ta
{
	class ServerAiComponentData : public CommonAiComponentData
	{
	public:
		explicit ServerAiComponentData(void) noexcept;
		virtual ~ServerAiComponentData(void) noexcept;

	public:
	};
}