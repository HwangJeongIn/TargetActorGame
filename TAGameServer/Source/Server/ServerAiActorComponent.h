#pragma once

#include "Common/CommonAiActorComponent.h"
#include "Common/KeyDefinition.h"
#include "Common/AiBehaviorTree.h"

namespace ta
{
	class AiPathPointPath;
}

namespace ta
{
	class ServerAiActorComponent : public CommonAiActorComponent
	{
	public:
		ServerAiActorComponent(void) noexcept;
		virtual ~ServerAiActorComponent(void) noexcept;

		virtual bool initializeData(const ComponentData* componentData) noexcept override final;
	
		const AiState& getCurrentAiState_(void) const noexcept;
		bool setCurrentAiState_(const AiState& aiState) noexcept;

		const AiClassType& getAiClassType_(void) const noexcept;
		bool setAiClass_(const AiClassType& aiClassType) noexcept;

		AiBehaviorTree& getAi(void) noexcept;

		virtual bool hasPathPointPath_(void) const noexcept override final;
		const AiPathPointPath* getAiPathPointPath_(void) const noexcept;
		AiPathPointPath* getAiPathPointPath_(void) noexcept;

	private:
		AiState _currentAiState;

		// 아래세개는 초기화때 말고 사실상 변할일이 없는 것들 
		AiClassType _aiClassType;
		AiBehaviorTree _ai;

		// 이 변수는 추후에 플레이중 변할 수도 있을것같다. // 여러가지 패스를 가진 Ai
		AiPathPointPath* _aiPathPointPath; // 없을 수 있다. // 가만히 서있는 NPC
	};
}
