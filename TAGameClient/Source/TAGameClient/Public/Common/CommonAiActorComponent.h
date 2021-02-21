#pragma once

#include "Common/ActorComponent.h"
#include "Common/Vector.h"
#include "Common/KeyDefinition.h"
#include <unordered_set>


namespace ta
{
	class CommonAiActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonAiActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;
		virtual ComponentData* packData_(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;

	protected:
		CommonAiActorComponent(void) noexcept;

	public:
		//bool _isActive; // 서버에서 처리하면될듯 클라가 깨우는건아니고 서버에서 섹터 움직일때 깨운다.
		
		// AiState _currentState; // 클라는 현재 어떤 state인지 알 필요없을 것같다. 그냥 시뮬레이션만 해주면될듯
		ActorKey _targetActorKey; // 서버에만 있는걸로 바뀔 수 있음
		float _attackRange; // 나중에 공격전에 범위 표시할때 클라에도 필요할듯

	};
}
