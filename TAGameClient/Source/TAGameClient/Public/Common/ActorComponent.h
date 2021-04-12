#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"
#include "Common/Lockable.h"
#include "Common/ActorDataPoolValue.h"
#include "Common/ComponentData.h"


namespace ta
{
	class CommonActor;
	class ActorKey;
}


namespace ta
{
	class ActorComponent : public Uncopyable, public Lockable, public ActorDataPoolValue
	{
	public:
		const ActorComponentType getActorComponentType(void) const noexcept;

		virtual ~ActorComponent(void) noexcept;

		virtual void onToPool(void) noexcept {}
		// 개별적으로 초기화되어야 한다. owner의 다른 컴포넌트는 초기화안되었을 수 있음
		virtual void onFromPool(void) noexcept {}
		// 최종적으로 초기화되고 업데이트 준비가 되면 호출. 이때는 owner의 모든 컴포넌트에 접근해도된다.
		virtual void onActive(void) noexcept {}

		CommonActor* getOwner(void) noexcept;
		const ActorKey& getOwnerActorKey(void) noexcept;
		void setOwner_(CommonActor* owner) noexcept;

		bool getComponentValid_(void) const noexcept;
		void setComponentValid_(const bool value) noexcept;

		// 데이터 정보가 세팅된다 Common에서 공통적인 데이터 초기화를 오버라이딩하고 최종적인 Client Server레벨에서 각 데이터를 초기화해준다. 
		virtual bool initializeData(const ComponentData* componentData) noexcept { return true; }
		virtual ComponentData* packData_(void) noexcept { return nullptr; }

	protected:
		// 자체로 생성 못함 무조건 Common이상으로 올라감
		ActorComponent(const ActorComponentType& actorComponentType) noexcept;
		
	private:
		// 이값은 일단 초기화되는 동안에는 true값이다.
		// 특정 액터에 할당되었다는 뜻이다. // 실제로 사용하기 위해서는 ActorDataPoolValue의 state가 using이여야 한다.
		bool _componentValid;

		ActorComponentType _actorComponentType;
		CommonActor* _owner;
	};
}
