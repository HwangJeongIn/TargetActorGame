#pragma once

#include "Common/CommonMoveActorSystem.h"
#include "RecastNavigation/Detour/DetourNavMesh.h"
#include "RecastNavigation/Detour/DetourNavMeshQuery.h"
#include <vector>


class dtNavMeshQuery;
class dtQueryFilter;
struct dtQueryResult;

namespace ta
{
	class ServerMoveActorComponent;
	class NavMeshPath;
	class NavMeshPoint;
}


namespace ta
{
	class ServerMoveActorSystem : public CommonMoveActorSystem
	{
	public:
		ServerMoveActorSystem(void) noexcept;
		virtual ~ServerMoveActorSystem(void) noexcept;

		virtual bool initialize(void) noexcept override final;

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


	public:
		// Navigation =========================================================================================================================================
		bool findPath(const ActorKey& targetActorKey, const Vector& startPos, const Vector& endPos, NavMeshPath& path) noexcept;

	private:
		bool preparePathFinding(const Vector& startPos, const Vector& endPos,
								const dtNavMeshQuery& query, const dtQueryFilter& queryFilter,
								Vector& recastStartPos, dtPolyRef& startPoly,
								Vector& recastEndPos, dtPolyRef& endPoly) const noexcept;


		bool generatePath(dtStatus findPathStatus, NavMeshPath& path,
						  const dtNavMeshQuery& query, const dtQueryFilter* queryFilter,
						  dtPolyRef startPoly, dtPolyRef endPoly,
						  const Vector& startPos, const Vector& endPos,
						  const Vector& recastStartPos, const Vector& recastEndPos,
						  dtQueryResult& pathResult) const noexcept;
		
		bool findStraightPathFromCorridor(const Vector& startPos, const Vector& endPos, 
										  const std::vector<dtPolyRef>& pathCorridor, std::vector<NavMeshPoint*>& pathPoints, std::vector<uint32>* customLinks) const noexcept;


		float calcSegmentCostOnPoly(const dtPolyRef& PolyID, const dtQueryFilter* queryFilter, const Vector& startPos, const Vector& endPos) const noexcept;

		// Navigation =========================================================================================================================================

	private:
		
		// dtQuerySpecialLinkFilter _defaultQuerySpecialLinkFilter;
		dtNavMesh* _detourNavMesh; 
		dtQueryFilter _defaultQueryFilter;
		const int32 _defaultMaxNodes;
		const float _defaultCostLimit;
		// Navigation =========================================================================================================================================
	};
}
