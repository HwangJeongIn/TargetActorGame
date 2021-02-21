#pragma once

#include "Common/ActorSystem.h"
#include "Common/KeyDefinition.h"
#include <unordered_set>


namespace ta
{
	class CommonActor;
	class Vector;
	class CommonMoveActorComponent;
}


namespace ta
{
	class CommonMoveActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonMoveActorSystem(void) noexcept;
		static const ActorSystemType getActorSystemType(void) noexcept;

		// process붙은 함수류는 서버에서 돌리는 Ai와 유저가 돌리는 Ai를 둘다 돌리기 위해서 만들었다.
		virtual bool processMoveActor(CommonActor* target
										 , const Vector& newPos
										 , const bool isForced) const noexcept;

		virtual bool processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept;
		virtual bool processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept;

		ActorKey getTargetActorFromSector(CommonMoveActorComponent* myMove
										  , const ActorType& actorType
										  , const bool searchNearSectors = false) const noexcept;

		ActorKey getTargetActorFromSector(CommonMoveActorComponent* myMove
										  , const std::vector<ActorType>& actorTypes
										  , const bool searchNearSectors = false) const noexcept;

		void getTargetActorsFromSector(CommonMoveActorComponent* myMove
									   , const std::vector<ActorType>& actorTypes
									   , std::vector<ActorKey>& output
									   , const bool searchNearSectors = false) const noexcept;


	protected:
		CommonMoveActorSystem(void) noexcept;

		bool moveActor(CommonActor* target
						  , const Vector& newPos
						  , const bool isForced = false
						  , std::unordered_set<SectorKey>* oldSectors = nullptr
						  , std::unordered_set<SectorKey>* newSectors = nullptr) const noexcept;


		bool move_(CommonMoveActorComponent* target, const Vector& newPos, const bool isForced = false) const noexcept;
		bool moveSector_(CommonMoveActorComponent* target, const SectorKey& newSectorKey) const noexcept;


	private:
		bool checkCanMove_(CommonMoveActorComponent* moveCom, const Vector& newPos) const noexcept;
	};
}
