#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/ActorDataPoolValue.h"
#include "Common/Lockable.h"
#include <unordered_map>

// 네트워크 관련부분 API와 구조는 => MSDN, Windows via C/C++, 게임서버프로그래밍교과서 코드 등을 참고하여 만들었음

namespace ta
{
	class ActorNetwork
	{
	public:
		ActorNetwork(void) noexcept;
		virtual ~ActorNetwork(void) noexcept;

	private:
	};
}
