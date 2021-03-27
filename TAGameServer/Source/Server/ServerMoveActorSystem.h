#pragma once

#include "Common/CommonMoveActorSystem.h"
#include "Common/Vector.h"
#include "Common/KeyDefinition.h"
#include "Common/Lockable.h"
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
	class PathPointPath;
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


		// Navigation =========================================================================================================================================
	public:
		virtual bool findPath(const ActorKey& targetActorKey, const Vector& startPos, const Vector& endPos, NavMeshPath& path) noexcept override final;
		virtual bool projectPointToNavMesh(const Vector& point, Vector& result) const noexcept override final;
		
	private:
		bool loadNavigationMeshFromBinary(void) noexcept;

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

	private:
		
		// dtQuerySpecialLinkFilter _defaultQuerySpecialLinkFilter;
		dtNavMesh* _detourNavMesh; 
		dtQueryFilter _defaultQueryFilter;

		const Vector _defaultRecastExtent;
		const Vector _defaultExtent;
		const int32 _defaultMaxNodes;
		const float _defaultCostLimit;
		// ====================================================================================================================================================

		// PathPoint ==========================================================================================================================================
	public:
		virtual bool loadPathPointPathSetFromXml(const fs::path filePath) noexcept override final;

		const PathPointPath* getPathPointPath(const PathPointPathKey& pathPointPathKey) const noexcept;
	private:

	private:
		// 로드시에만 사용하는 락 // 로드후에는 변경안되기 떄문에 락안건다.
		LockableObject _pathPointPathSetLoadLock;
		std::unordered_map<PathPointPathKey, PathPointPath*> _pathPointPathSet;

		// ====================================================================================================================================================
	};
}
