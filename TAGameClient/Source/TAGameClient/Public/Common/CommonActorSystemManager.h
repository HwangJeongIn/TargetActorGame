#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include <vector>


namespace ta
{
	class CommonActorManager;
	class ActorSystem;
}


namespace ta
{
	class CommonActorSystemManager : public Uncopyable
	{
	public:
		virtual bool initialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		ActorSystem* getActorSystem(const ActorSystemType systemType) noexcept;

		virtual ~CommonActorSystemManager(void) noexcept;
	protected:
		explicit CommonActorSystemManager(CommonActorManager* actorManager) noexcept;

	protected:

		// 풀같은 경우는 연속적인 메모리에 할당해야하기 때문에 포인터 배열로 하지 않았음 각 CommonXXXActorComponent 배열이 존재

		// 인덱스 : ActorSystemType 값 
		// 여기서는 ActorSystem포인터 지워야 하기 때문에 ActorSystem소멸자는 public으로 선언했다.
		std::vector<ActorSystem*> _actorSystems;

		CommonActorManager* _actorManager;
	};																																	 

}
