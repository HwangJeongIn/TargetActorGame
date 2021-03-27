#include "Common/AiBehaviorTree.h"
#include "Common/CommonAiActorComponent.h"

namespace ta
{
	AiBTNodeBlock AiBehaviorTree::MonsterAi;
	AiBTNodeBlock AiBehaviorTree::NpcAi;

	// Condition 객체
	AiBTConditionHasTarget AiBehaviorTree::HasTarget(false);
	AiBTConditionHasTarget AiBehaviorTree::HasNoTarget(true);
	AiBTConditionIsInAttackRange AiBehaviorTree::IsInAttackRange(false);
	AiBTConditionIsInAttackRange AiBehaviorTree::IsNotInAttackRange(true);

	AiBTConditionHasPathPointPath AiBehaviorTree::HasPathPointPath(false);
}


namespace ta
{

	AiBehaviorTree::AiBehaviorTree(void) noexcept
		: _rootNode(nullptr)
	{
	}

	AiBehaviorTree::~AiBehaviorTree(void) noexcept
	{
		
	}

	void AiBehaviorTree::initializeAllAi(void) noexcept
	{
		initializeMonsterAi();
		initializeNpcAi();
	}

	void AiBehaviorTree::initializeMonsterAi(void) noexcept
	{
		AiBTNodeSelector* baseBlock = new AiBTNodeSelector;
		MonsterAi.addChildNode(baseBlock);
		{
			AiBTNodeSelector* detectOrInteractBlock = new AiBTNodeSelector;
			baseBlock->addChildNode(detectOrInteractBlock);
			{
				AiBTNodeExecutionDetectTargetPlayer* detectExecution = new AiBTNodeExecutionDetectTargetPlayer(2.0f);
				detectOrInteractBlock->addExecution(detectExecution);
				{
					detectExecution->addCondition(&AiBehaviorTree::HasNoTarget);
				}

				AiBTNodeSelector* attackOrChaseBlock = new AiBTNodeSelector;
				detectOrInteractBlock->addChildNode(attackOrChaseBlock);
				{
					attackOrChaseBlock->addCondition(&AiBehaviorTree::HasTarget);

					AiBTNodeBlock* attackBlock = new AiBTNodeBlock;
					attackOrChaseBlock->addChildNode(attackBlock);
					{
						attackBlock->addCondition(&AiBehaviorTree::IsInAttackRange);

						AiBTNodeAttackTarget* attackNode = new AiBTNodeAttackTarget;
						attackBlock->addChildNode(attackNode);
					}

					AiBTNodeBlock* chaseBlock = new AiBTNodeBlock;
					attackOrChaseBlock->addChildNode(chaseBlock);
					{
						chaseBlock->addCondition(&AiBehaviorTree::IsNotInAttackRange);

						AiBTNodeMoveToTarget* chaseNode = new AiBTNodeMoveToTarget;
						chaseBlock->addChildNode(chaseNode);
					}
				}
			}

			AiBTNodeSequence* idleBlock = new AiBTNodeSequence;
			baseBlock->addChildNode(idleBlock);
			{
				AiBTNodeWait* waitNode = new AiBTNodeWait(3.0f);
				idleBlock->addChildNode(waitNode);
				AiBTNodeMoveToRandomPoint* moveToRandomPointNode = new AiBTNodeMoveToRandomPoint;
				idleBlock->addChildNode(moveToRandomPointNode);
			}
		}
	}

	void AiBehaviorTree::initializeNpcAi(void) noexcept
	{
		AiBTNodeMoveToPathPoint* moveToPathPointBlock = new AiBTNodeMoveToPathPoint;
		NpcAi.addChildNode(moveToPathPointBlock);
		moveToPathPointBlock->addCondition(&AiBehaviorTree::HasPathPointPath);
	}

	bool AiBehaviorTree::tick(const ActorKey& actorKey, const float deltaTime) noexcept
	{
		if (nullptr == _rootNode)
		{
			TA_ASSERT_DEV(false, "Ai가 초기화되지 않았습니다.");
			return false;
		}
		_rootNode->executeNode(actorKey, deltaTime);

		return true;
	}

	void AiBehaviorTree::initializeAsMonster_(void) noexcept
	{
		_rootNode = &(AiBehaviorTree::MonsterAi);
	}

	void AiBehaviorTree::initializeAsNpc_(void) noexcept
	{
		_rootNode = &(AiBehaviorTree::NpcAi);
	}

}