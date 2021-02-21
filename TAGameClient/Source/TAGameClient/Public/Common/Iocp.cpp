#include "Common/Iocp.h"
#include "Common/Socket.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	Iocp::Iocp(void) noexcept
	{
		_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE // 생성할때는 무조건 이 파라미터로
										 , NULL // 존재하는 IOCP핸들 => 새로 만들려면 NULL
										 , NULL
										 , 0); // IOCP 객체가 동시에 컨트롤 할 수 있는 스레드의 개수
											   // 0을 넣으면 코어 개수만큼 사용하겠다는 뜻
	}

	bool Iocp::bindSocket(Socket& socket, const ActorKey& actorKey) noexcept
	{

		// 소켓 연결
		HANDLE result = CreateIoCompletionPort((HANDLE)socket._handle // 생성할때는 무조건 이 파라미터로
											   , _handle // 존재하는 IOCP핸들 => NULL
											   , (ULONG_PTR)actorKey.getKeyValue() // 보통 클라이언트 정보를 보관하는 포인터나 플레이어 ID등으로 사용한다 
																				   // 통지받을때 이 키를 가지고 어떤 곳이 완료되었는지 확인 가능하다.
											   , 0); // 무시
		if (0 == result)
		{
			TA_ASSERT_DEV(false, "bindSocket fail : %s", GetLastErrorAsString().data());
			return false;
		}

		return true;
	}
}

