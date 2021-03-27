#pragma once

#include "Common/ActorSystem.h"
#include "Common/KeyDefinition.h"
#include <unordered_set>
#include <filesystem>


namespace fs = std::filesystem;


#ifndef TA_SERVER
class dtNavMesh;
#endif

namespace ta
{
#if defined(TA_SERVER) && !defined(TA_CLIENT_IN_SERVER)
	class dtNavMesh;
	extern const fs::path NavigationMeshFilePath;
	extern const fs::path PathPointFilePath;
#elif !defined(TA_SERVER)
	extern fs::path NavigationMeshFilePath;
	extern fs::path PathPointFilePath;
#endif

	class CommonActor;
	class Vector;
	class CommonMoveActorComponent;
	class Serializer;
	class NavMeshPath;
}


namespace ta
{
	class CommonMoveActorSystem : public ActorSystem
	{
	public:
		virtual ~CommonMoveActorSystem(void) noexcept;
		static const ActorSystemType getActorSystemType(void) noexcept;

#ifndef TA_CLIENT_IN_SERVER
		static bool serializeNavigationMesh(Serializer& Ar, dtNavMesh*& DetourNavMesh) noexcept;
		static void serializeRecastMeshTile(Serializer& Ar, int32 NavMeshVersion, unsigned char*& TileData, int32& TileDataSize) noexcept;
		static void serializeCompressedTileCacheData(Serializer& Ar, int32 NavMeshVersion, unsigned char*& CompressedData, int32& CompressedDataSize) noexcept;
#endif

		virtual bool loadPathPointPathSetFromXml(const fs::path filePath) noexcept;

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

		virtual bool findPath(const ActorKey& targetActorKey, const Vector& startPos, const Vector& endPos, NavMeshPath& path) noexcept;
		virtual bool projectPointToNavMesh(const Vector& point, Vector& result) const noexcept;

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
