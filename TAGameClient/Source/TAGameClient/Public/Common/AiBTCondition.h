#pragma once


namespace ta
{
	class AiBehaviorTree;
	class ActorKey;
}


namespace ta
{
	class AiBTCondition
	{
	public:
		explicit AiBTCondition(bool isNot) noexcept;
		virtual ~AiBTCondition(void) noexcept;

		virtual bool checkConditionDetail(const ActorKey& actorKey) const noexcept = 0;

		bool checkCondition(const ActorKey& actorKey) const noexcept;

	public:
		bool _isNot;
	};
}


namespace ta
{
	class AiBTConditionHasTarget : public AiBTCondition
	{
	public:
		explicit AiBTConditionHasTarget(bool isNot) noexcept;
		virtual ~AiBTConditionHasTarget(void) noexcept;

		virtual bool checkConditionDetail(const ActorKey& actorKey) const noexcept override final;
	};
}


namespace ta
{
	class AiBTConditionIsInAttackRange : public AiBTCondition
	{
	public:
		AiBTConditionIsInAttackRange(bool isNot) noexcept;
		virtual ~AiBTConditionIsInAttackRange(void) noexcept;

		virtual bool checkConditionDetail(const ActorKey& actorKey) const noexcept override final;
	};
}
