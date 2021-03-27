#pragma once
#include "Common/CommonBase.h"
#include "Common/AiBlackboard.h"
#include <vector>


namespace ta
{
	class ActorKey;
	class AiBehaviorTree;
	class AiBTCondition;
	class AiBTNodeExecution;
}


namespace ta
{
	enum class AiBTNodeState : uint8
	{
		Success = 0
		, Failure
		, Continue
	};
}


namespace ta
{
	class AiBTNodeBase
	{
	public:
		AiBTNodeBase(void) noexcept;
		virtual ~AiBTNodeBase(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept;

		bool addCondition(AiBTCondition* condition) noexcept;
		bool checkCondition(const ActorKey& actorKey) const noexcept;

	protected:
		// 노드는 무조건 전역 변수 or static 변수가 들어가야한다. 여러곳에서 같이 쓸 예정
		std::vector<AiBTCondition*> _conditions;
	};
}


namespace ta
{
	class AiBTNode : public AiBTNodeBase
	{
	public:
		AiBTNode(void) noexcept;
		virtual ~AiBTNode(void) noexcept;

		AiBTNodeState executeNode(const ActorKey& actorKey, const float& deltaTime) noexcept;

		bool addExecution(AiBTNodeExecution* execution) noexcept;

	protected:
		std::vector<AiBTNodeExecution*> _nodeExecutionList;
	};
}


namespace ta
{
	class AiBTNodeExecution : public AiBTNodeBase
	{
	public:
		explicit AiBTNodeExecution(const float interval) noexcept;
		virtual ~AiBTNodeExecution(void) noexcept;
		void executeNode(const ActorKey& actorKey, const float& deltaTime) noexcept;

	protected:
		float _interval;
		float _currentTime;
	};
}


namespace ta
{
	class AiBTNodeComposite : public AiBTNode
	{
	public:
		AiBTNodeComposite(void) noexcept;
		virtual ~AiBTNodeComposite(void) noexcept;

		bool addChildNode(AiBTNode* childNode) noexcept;
	protected:
		std::vector<AiBTNode*> _childNodes;
	};
}


namespace ta
{
	// 수행해야하는 노드중 하나만 성공해도 성공 반환 / 모두 실패하면 실패반환
	class AiBTNodeSelector : public AiBTNodeComposite
	{
	public:
		AiBTNodeSelector(void) noexcept;
		virtual ~AiBTNodeSelector(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}


namespace ta
{
	// 쭉 수행 // Continue나오면 리턴 // Root노드 // 단순히 병렬적으로 수행해야할 노드들 묶을때 사용
	class AiBTNodeBlock : public AiBTNodeComposite
	{
	public:
		AiBTNodeBlock(void) noexcept;
		virtual ~AiBTNodeBlock(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}


namespace ta
{
	// 수행해야하는 노드가 모두 성공해야 성공 반환 / 하나라도 실패하면 실패반환
	class AiBTNodeSequence : public AiBTNodeComposite
	{
	public:
		AiBTNodeSequence(void) noexcept;
		virtual ~AiBTNodeSequence(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;
	};
}


namespace ta
{
	// 서버에서 자주말고 시간두고 수행해주자 // 클라에서 타게팅하고 서버로 올리는것보다 안전
	// 충돌처리(콜리전)나 섹터 뒤져서 타겟팅하는것 중에 정해야한다. // 일단 충돌처리 없으니까 섹터 보고 처리하자
	class AiBTNodeExecutionDetectTargetPlayer : public AiBTNodeExecution
	{
	public:
		explicit AiBTNodeExecutionDetectTargetPlayer(const float interval) noexcept;
		virtual ~AiBTNodeExecutionDetectTargetPlayer(void) noexcept;

		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;
	};
}


namespace ta
{
	// 시퀀스 안에서만 사용
	class AiBTNodeWait : public AiBTNode
	{
	public:
		explicit AiBTNodeWait(const float interval) noexcept;
		virtual ~AiBTNodeWait(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	protected:
		float _interval;
		float _currentTime;
	};
}


namespace ta
{
	class AiBTNodeAttackTarget : public AiBTNode
	{
	public:
		AiBTNodeAttackTarget(void) noexcept;
		virtual ~AiBTNodeAttackTarget(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}


namespace ta
{
	class AiBTNodeMoveToTarget : public AiBTNode
	{
	public:
		AiBTNodeMoveToTarget(void) noexcept;
		virtual ~AiBTNodeMoveToTarget(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}


namespace ta
{
	class AiBTNodeMoveToRandomPoint : public AiBTNode
	{
	public:
		AiBTNodeMoveToRandomPoint(void) noexcept;
		virtual ~AiBTNodeMoveToRandomPoint(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}


namespace ta
{
	class AiBTNodeMoveToPathPoint : public AiBTNode
	{
	public:
		AiBTNodeMoveToPathPoint(void) noexcept;
		virtual ~AiBTNodeMoveToPathPoint(void) noexcept;
		virtual AiBTNodeState executeNodeDetail(const ActorKey& actorKey, const float& deltaTime) noexcept override final;

	};
}
