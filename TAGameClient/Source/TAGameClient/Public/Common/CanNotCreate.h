#pragma once

namespace ta
{
	// 생성할 수 없는 클래스에 사용
	class CanNotCreate
	{
	private:
		CanNotCreate(void) noexcept {}
		//virtual ~CanNotCreate(void) noexcept {}
		CanNotCreate(const CanNotCreate&) {}
		CanNotCreate& operator=(const CanNotCreate&) {}
	};
}
