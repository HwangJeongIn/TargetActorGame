#pragma once

#include "Common/CommonAiActorComponent.h"
#include "Common/KeyDefinition.h"
#include "Common/AiBehaviorTree.h"


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

	private:
		AiState _currentAiState;
		AiClassType _aiClassType;
		AiBehaviorTree _ai;
	};
}
