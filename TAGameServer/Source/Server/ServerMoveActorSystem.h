#pragma once

#include "Common/CommonMoveActorSystem.h"


namespace ta
{
	class ServerMoveActorComponent;
}


namespace ta
{
	class ServerMoveActorSystem : public CommonMoveActorSystem
	{
	public:
		ServerMoveActorSystem(void) noexcept;
		virtual ~ServerMoveActorSystem(void) noexcept;

		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;


		virtual bool processMoveActor(CommonActor* target
										 , const Vector& newPos
										 , const bool isForced) const noexcept override final;

		virtual bool processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept override final;
		virtual bool processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept override final;


		bool respondMoveActor(CommonActor* target, const Vector& newPos) const noexcept;

		bool moveActorAndNotify(CommonActor* target, const Vector& newPos, const bool isForced = false, const bool notifyToClient = true) const noexcept;
		void moveToCurrentPosition(CommonActor* target) const noexcept;

		bool moveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept;
		bool moveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept;



	private:
		bool processPostMove(CommonActor* target, const Vector& newPos) const noexcept;
		bool processNearSectors(CommonActor* targetActor
								, const std::unordered_set<SectorKey>& fromSectors
								, const std::unordered_set<SectorKey>& toSectors) const noexcept;

		void moveActorInClient(ServerMoveActorComponent* clientActorMoveCom, const ActorKey& targetActorKey) const noexcept;
		void createActorInClient(ServerMoveActorComponent* clientActorMoveCom, const ActorKey& targetActorKey, const ActorType& actorType) const noexcept;
		void destroyActorInClient(ServerMoveActorComponent* clientActorMoveCom, const ActorKey& targetActorKey) const noexcept;

		void convertToActorKeyWithActorType(const std::unordered_set<ActorKey>& input, std::unordered_map<ActorKey, ActorType>& output) const noexcept;

		bool activateAiIfDisabled(const ActorKey& targetActorKey) const noexcept;
		bool deactivateAiIfNotDisabled(const ActorKey& targetActorKey) const noexcept;
	};
}
