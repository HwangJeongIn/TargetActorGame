#pragma once
#include "Common/CommonBase.h"
#include "Common/AiBlackboard.h"
#include "Common/AiBTCondition.h"
#include "Common/AiBTNode.h"
#include <vector>



namespace ta
{
	class AiBehaviorTree
	{
	public:
		AiBehaviorTree(void) noexcept;
		~AiBehaviorTree(void) noexcept;

		// ai roots
		static AiBTNodeBlock MonsterAi; // 기본적인 몬스터
		static AiBTNodeBlock NpcAi; // 기본적인 엔피씨

		static void initializeAllAi(void) noexcept;
		
		// conditions
		static AiBTConditionHasTarget HasTarget;
		static AiBTConditionHasTarget HasNoTarget;
		static AiBTConditionIsInAttackRange IsInAttackRange;
		static AiBTConditionIsInAttackRange IsNotInAttackRange;
		
		static AiBTConditionHasPathPointPath HasPathPointPath;



		bool tick(const ActorKey& actorKey, const float deltaTime) noexcept;
		void initializeAsMonster_(void) noexcept;
		void initializeAsNpc_(void) noexcept;

	private:
		static void initializeMonsterAi(void) noexcept;
		static void initializeNpcAi(void) noexcept;

	private:
		//AiBlackboard _blackBoard; // 그냥 사용안하고 컴포넌트에서 데이터 빼서 사용하기로 // 사용하면 컴포넌트 데이터와 동기화시켜야함..
		AiBTNodeBlock* _rootNode;
	};
}
