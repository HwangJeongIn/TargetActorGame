#include "Common/AiBTNode.h"
#include "Common/AiBTCondition.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonAiActorComponent.h"
#include "Common/CommonCharacterActorComponent.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/CommonActionActorSystem.h"
#include "Common/CommonAiActorSystem.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/ScopedLock.h"



namespace ta
{
	AiBTNodeBase::AiBTNodeBase(void) noexcept
	{
	}

	AiBTNodeBase::~AiBTNodeBase(void) noexcept
	{
		// condition은 static변수 사용할거라서 따로 삭제하면 안된다.
		_conditions.clear();
	}

	AiBTNodeState AiBTNodeBase::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		return AiBTNodeState::Success;
	}

	bool AiBTNodeBase::addCondition(AiBTCondition* condition) noexcept
	{
		_conditions.push_back(condition);
		return true;
	}

	bool AiBTNodeBase::checkCondition(const ActorKey& actorKey) const noexcept
	{
		const uint32 count = _conditions.size();
		for (uint32 index = 0; index < count; ++index)
		{
			if (false == _conditions[index]->checkCondition(actorKey))
			{
				return false;
			}
		}

		return true;
	}
}


namespace ta
{
	AiBTNode::AiBTNode(void) noexcept
	{
		const uint32 count = _nodeExecutionList.size();
		for (uint32 index = 0; index < count; ++index)
		{
			delete _nodeExecutionList[index];
		}

		_nodeExecutionList.clear();
	}

	AiBTNode::~AiBTNode(void) noexcept
	{
	}

	AiBTNodeState AiBTNode::executeNode(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		if (false == checkCondition(actorKey))
		{
			return AiBTNodeState::Failure;
		}

		const uint32 count = _nodeExecutionList.size();
		for (uint32 index = 0; index < count; ++index)
		{
			_nodeExecutionList[index]->executeNode(actorKey, deltaTime);
		}

		return executeNodeDetail(actorKey, deltaTime);
	}
	
	bool AiBTNode::addExecution(AiBTNodeExecution* execution) noexcept
	{
		_nodeExecutionList.push_back(execution);
		return true;
	}
}


namespace ta
{
	AiBTNodeExecution::AiBTNodeExecution(const float interval) noexcept
		: _interval(interval)
		, _currentTime(0.0f)
	{
	}

	AiBTNodeExecution::~AiBTNodeExecution(void) noexcept
	{
	}

	void AiBTNodeExecution::executeNode(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		if (false == checkCondition(actorKey))
		{
			return;
		}

		_currentTime += deltaTime;
		if (_currentTime < _interval)
		{
			return;
		}

		_currentTime = 0.0f;

		executeNodeDetail(actorKey, deltaTime);
	}
}


namespace ta
{
	AiBTNodeComposite::AiBTNodeComposite(void) noexcept
	{
	}

	AiBTNodeComposite::~AiBTNodeComposite(void) noexcept
	{
		const uint32 count = _childNodes.size();
		for (uint32 index = 0; index < count; ++index)
		{
			delete _childNodes[index];
		}

		_childNodes.clear();
	}

	bool AiBTNodeComposite::addChildNode(AiBTNode* childNode) noexcept
	{
		_childNodes.push_back(childNode);
		return true;
	}
}


namespace ta
{
	AiBTNodeBlock::AiBTNodeBlock(void) noexcept
	{
	}

	AiBTNodeBlock::~AiBTNodeBlock(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeBlock::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		const uint32 count = _childNodes.size();
		AiBTNodeState childState = AiBTNodeState::Success;
		for (uint32 index = 0; index < count; ++index)
		{
			childState = _childNodes[index]->executeNode(actorKey, deltaTime);
			if (AiBTNodeState::Continue == childState)
			{
				return AiBTNodeState::Continue;
			}
		}

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeSelector::AiBTNodeSelector(void) noexcept
	{
	}

	AiBTNodeSelector::~AiBTNodeSelector(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeSelector::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		const uint32 count = _childNodes.size();
		AiBTNodeState childState = AiBTNodeState::Success;
		for (uint32 index = 0; index < count; ++index)
		{
			childState = _childNodes[index]->executeNode(actorKey, deltaTime);
			if (AiBTNodeState::Continue == childState)
			{
				return AiBTNodeState::Continue;
			}
			else if (AiBTNodeState::Success == childState)
			{
				return AiBTNodeState::Success;
			}
		}

		return AiBTNodeState::Failure;
	}
}


namespace ta
{
	AiBTNodeSequence::AiBTNodeSequence(void) noexcept
	{
	}

	AiBTNodeSequence::~AiBTNodeSequence(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeSequence::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		const uint32 count = _childNodes.size();
		AiBTNodeState childState = AiBTNodeState::Success;
		for (uint32 index = 0; index < count; ++index)
		{
			childState = _childNodes[index]->executeNode(actorKey, deltaTime);
			if (AiBTNodeState::Continue == childState)
			{
				return AiBTNodeState::Continue;
			}
			else if (AiBTNodeState::Failure == childState)
			{
				return AiBTNodeState::Failure;
			}
		}

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeExecutionDetectTargetPlayer::AiBTNodeExecutionDetectTargetPlayer(const float interval) noexcept
		: AiBTNodeExecution(interval)
	{
	}

	AiBTNodeExecutionDetectTargetPlayer::~AiBTNodeExecutionDetectTargetPlayer(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeExecutionDetectTargetPlayer::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(actorKey);
		if (nullptr == moveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonAiActorSystem* aiSystem = GetActorSystem<CommonAiActorSystem>();
		if (false == aiSystem->processDetectTargetPlayer(aiCom, moveCom))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeWait::AiBTNodeWait(const float interval) noexcept
		: _interval(interval)
		, _currentTime(0.0f)
	{
	}

	AiBTNodeWait::~AiBTNodeWait(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeWait::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		_currentTime += deltaTime;
		if (_currentTime < _interval)
		{
			return AiBTNodeState::Continue;
		}
		_currentTime = 0.0f;

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeAttackTarget::AiBTNodeAttackTarget(void) noexcept
	{
	}

	AiBTNodeAttackTarget::~AiBTNodeAttackTarget(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeAttackTarget::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonCharacterActorComponent* characterCom = GetActorComponent<CommonCharacterActorComponent>(actorKey);
		if (nullptr == characterCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		ActorKey targetActorKey;
		{
			ScopedLock aiComLock(aiCom, true);
			targetActorKey = aiCom->_targetActorKey;
		}

		if (false == targetActorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonCharacterActorComponent* targetCharacterCom = GetActorComponent<CommonCharacterActorComponent>(targetActorKey);
		if (nullptr == targetCharacterCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonActionActorSystem* actionSystem = GetActorSystem<CommonActionActorSystem>();
		if (false == actionSystem->processAttackTarget(characterCom, targetCharacterCom))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeMoveToTarget::AiBTNodeMoveToTarget(void) noexcept
	{
	}

	AiBTNodeMoveToTarget::~AiBTNodeMoveToTarget(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeMoveToTarget::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(actorKey);
		if (nullptr == moveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		ActorKey targetActorKey;
		{
			ScopedLock aiComLock(aiCom, true);
			targetActorKey = aiCom->_targetActorKey;
		}

		if (false == targetActorKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonMoveActorComponent* targetMoveCom = GetActorComponent<CommonMoveActorComponent>(targetActorKey);
		if (nullptr == targetMoveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonMoveActorSystem* moveSystem = GetActorSystem<CommonMoveActorSystem>();
		if (false == moveSystem->processMoveToTarget(moveCom, targetMoveCom))
		{
			//targetMoveCom
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		return AiBTNodeState::Success;
	}
}


namespace ta
{
	AiBTNodeMoveToRandomPoint::AiBTNodeMoveToRandomPoint(void) noexcept
	{
	}

	AiBTNodeMoveToRandomPoint::~AiBTNodeMoveToRandomPoint(void) noexcept
	{
	}

	AiBTNodeState AiBTNodeMoveToRandomPoint::executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept
	{
		CommonAiActorComponent* aiCom = GetActorComponent<CommonAiActorComponent>(actorKey);
		if (nullptr == aiCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonMoveActorComponent* moveCom = GetActorComponent<CommonMoveActorComponent>(actorKey);
		if (nullptr == moveCom)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		CommonMoveActorSystem* moveSystem = GetActorSystem<CommonMoveActorSystem>();
		if (false == moveSystem->processMoveToRandomPoint(moveCom))
		{
			//targetMoveCom
			TA_ASSERT_DEV(false, "비정상입니다.");
			return AiBTNodeState::Failure;
		}

		return AiBTNodeState::Success;
	}
}
