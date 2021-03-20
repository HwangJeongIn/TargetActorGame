#include "Common/CommonMoveActorSystem.h"
#include "Common/CommonMoveActorComponent.h"
#include "Common/CommonActor.h"
#include "Common/ScopedLock.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/Sector.h"
#include "Common/Serializer.h"

#if defined(TA_SERVER) && !defined(TA_CLIENT_IN_SERVER)
#include "RecastNavigation/Detour/DetourNavMesh.h"
#include "RecastNavigation/Detour/DetourNavMeshQuery.h"
#include "RecastNavigation/NavMeshPoint.h"
#include "RecastNavigation/NavMeshPath.h"
#elif !defined(TA_SERVER)
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"
#endif



namespace ta
{
#ifdef TA_SERVER
	const fs::path NavigationMeshPath = fs::absolute(L"./../../TAGameClient/NavigationMesh");
#else
	fs::path NavigationMeshPath = "";
#endif
}

namespace ta
{
	CommonMoveActorSystem::~CommonMoveActorSystem(void) noexcept
	{
	}

	const ActorSystemType CommonMoveActorSystem::getActorSystemType(void) noexcept
	{
		return ActorSystemType::MoveSystem;
	}

#ifndef TA_CLIENT_IN_SERVER
	bool CommonMoveActorSystem::serializeNavigationMesh(Serializer& Ar, dtNavMesh*& DetourNavMesh) noexcept
	{
		// 쓰려는데 없다? => 오류
		if (true == Ar.hasModeFlag(SerializerMode::Write) && DetourNavMesh == NULL)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		// All we really need to do is read/write the data blob for each tile

		if (true == Ar.hasModeFlag(SerializerMode::Read))
		{
			// allocate the navmesh object	
			if (DetourNavMesh != nullptr)
			{
				dtFreeNavMesh(DetourNavMesh);
			}
			DetourNavMesh = nullptr;

			DetourNavMesh = dtAllocNavMesh();

			if (DetourNavMesh == NULL)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}
		}

		int32 NumTiles = 0;
		std::vector<int32> TilesToSave;

		if (true == Ar.hasModeFlag(SerializerMode::Write))
		{
			TilesToSave.reserve(DetourNavMesh->getMaxTiles());

			//if (NavMeshOwner->SupportsStreaming() && !IsRunningCommandlet())
			//{
			//	// We save only tiles that belongs to this level
			//	GetNavMeshTilesIn(NavMeshOwner->GetNavigableBoundsInLevel(NavMeshOwner->GetLevel()), TilesToSave);
			//}
			//else
			{
				// Otherwise all valid tiles
				dtNavMesh const* ConstNavMesh = DetourNavMesh;
				for (int i = 0; i < ConstNavMesh->getMaxTiles(); ++i)
				{
					const dtMeshTile* Tile = ConstNavMesh->getTile(i);
					if (Tile != NULL && Tile->header != NULL && Tile->dataSize > 0)
					{
						TilesToSave.push_back(i);
					}
				}
			}

			NumTiles = TilesToSave.size();
		}

		Ar << NumTiles;

		dtNavMeshParams Params = *DetourNavMesh->getParams();
		Ar << Params.orig[0] << Params.orig[1] << Params.orig[2];
		Ar << Params.tileWidth;				///< The width of each tile. (Along the x-axis.)
		Ar << Params.tileHeight;			///< The height of each tile. (Along the z-axis.)
		Ar << Params.maxTiles;				///< The maximum number of tiles the navigation mesh can contain.
		Ar << Params.maxPolys;

		if (true == Ar.hasModeFlag(SerializerMode::Read))
		{
			//// at this point we can tell whether navmesh being loaded is in line
			//// ARecastNavMesh's params. If not, just skip it.
			//// assumes tiles are rectangular
			//const float ActorsTileSize = float(int32(NavMeshOwner->TileSizeUU / NavMeshOwner->CellSize) * NavMeshOwner->CellSize);

			//if (ActorsTileSize != Params.tileWidth)
			//{
			//	// just move archive position
			//	ReleaseDetourNavMesh();

			//	for (int i = 0; i < NumTiles; ++i)
			//	{
			//		dtTileRef TileRef = MAX_uint64;
			//		int32 TileDataSize = 0;
			//		Ar << TileRef << TileDataSize;

			//		if (TileRef == MAX_uint64 || TileDataSize == 0)
			//		{
			//			continue;
			//		}

			//		unsigned char* TileData = NULL;
			//		TileDataSize = 0;
			//		SerializeRecastMeshTile(Ar, NavMeshVersion, TileData, TileDataSize);
			//		if (TileData != NULL)
			//		{
			//			dtMeshHeader* const TileHeader = (dtMeshHeader*)TileData;
			//			dtFree(TileHeader);

			//			//
			//			if (Ar.UE4Ver() >= VER_UE4_ADD_MODIFIERS_RUNTIME_GENERATION &&
			//				(Ar.EngineVer().GetMajor() != 4 || Ar.EngineVer().GetMinor() != 7)) // Merged package from 4.7 branch
			//			{
			//				unsigned char* ComressedTileData = NULL;
			//				int32 CompressedTileDataSize = 0;
			//				SerializeCompressedTileCacheData(Ar, NavMeshVersion, ComressedTileData, CompressedTileDataSize);
			//				dtFree(ComressedTileData);
			//			}
			//		}
			//	}
			//}
			//else
			{
				// regular loading
				dtStatus Status = DetourNavMesh->init(&Params);
				if (dtStatusFailed(Status))
				{
					TA_ASSERT_DEV(false, "비정상");
				}

				for (int i = 0; i < NumTiles; ++i)
				{
					dtTileRef TileRef = _MAX_UINT64;
					int32 TileDataSize = 0;
					Ar << TileRef << TileDataSize;

					if (TileRef == _MAX_UINT64 || TileDataSize == 0)
					{
						continue;
					}

					unsigned char* TileData = NULL;
					TileDataSize = 0;
					serializeRecastMeshTile(Ar, 13, TileData, TileDataSize);

					if (TileData != NULL)
					{
						dtMeshHeader* const TileHeader = (dtMeshHeader*)TileData;
						DetourNavMesh->addTile(TileData, TileDataSize, DT_TILE_FREE_DATA, TileRef, NULL);

						//// Serialize compressed tile cache layer
						//if (Ar.UE4Ver() >= VER_UE4_ADD_MODIFIERS_RUNTIME_GENERATION &&
						//	(Ar.EngineVer().GetMajor() != 4 || Ar.EngineVer().GetMinor() != 7)) // Merged package from 4.7 branch
						//{
						//	uint8* ComressedTileData = nullptr;
						//	int32 CompressedTileDataSize = 0;
						//	SerializeCompressedTileCacheData(Ar, NavMeshVersion, ComressedTileData, CompressedTileDataSize);
						//
						//	if (CompressedTileDataSize > 0)
						//	{
						//		AddTileCacheLayer(TileHeader->x, TileHeader->y, TileHeader->layer,
						//						  FNavMeshTileData(ComressedTileData, CompressedTileDataSize, TileHeader->layer, Recast2UnrealBox(TileHeader->bmin, TileHeader->bmax)));
						//	}
						//}
					}
				}
			}
		}
		else if (true == Ar.hasModeFlag(SerializerMode::Write))
		{
			//const bool bSupportsRuntimeGeneration = NavMeshOwner->SupportsRuntimeGeneration();
			dtNavMesh const* ConstNavMesh = DetourNavMesh;

			for (int TileIndex : TilesToSave)
			{
				const dtMeshTile* Tile = ConstNavMesh->getTile(TileIndex);
				dtTileRef TileRef = ConstNavMesh->getTileRef(Tile);
				int32 TileDataSize = Tile->dataSize;
				Ar << TileRef << TileDataSize;

				unsigned char* TileData = Tile->data;
				serializeRecastMeshTile(Ar, 13, TileData, TileDataSize);

				//// Serialize compressed tile cache layer only if navmesh requires it
				//{
				//	FNavMeshTileData TileCacheLayer;
				//	uint8* CompressedData = nullptr;
				//	int32 CompressedDataSize = 0;
				//	if (bSupportsRuntimeGeneration)
				//	{
				//		TileCacheLayer = GetTileCacheLayer(Tile->header->x, Tile->header->y, Tile->header->layer);
				//		CompressedData = TileCacheLayer.GetDataSafe();
				//		CompressedDataSize = TileCacheLayer.DataSize;
				//	}
				//
				//	SerializeCompressedTileCacheData(Ar, NavMeshVersion, CompressedData, CompressedDataSize);
				//}
			}
		}
		return true;
	}


	void CommonMoveActorSystem::serializeRecastMeshTile(Serializer& Ar, int32 NavMeshVersion, unsigned char*& TileData, int32& TileDataSize) noexcept
	{
		// The strategy here is to serialize the data blob that is passed into addTile()
		// @see dtCreateNavMeshData() for details on how this data is laid out

		int32 totVertCount;
		int32 totPolyCount;
		int32 maxLinkCount;
		int32 detailMeshCount;
		int32 detailVertCount;
		int32 detailTriCount;
		int32 bvNodeCount;
		int32 offMeshConCount;
		int32 offMeshSegConCount;
		int32 clusterCount;

		if (true == Ar.hasModeFlag(SerializerMode::Write))
		{
			// fill in data to write
			dtMeshHeader* const H = (dtMeshHeader*)TileData;
			totVertCount = H->vertCount;
			totPolyCount = H->polyCount;
			maxLinkCount = H->maxLinkCount;
			detailMeshCount = H->detailMeshCount;
			detailVertCount = H->detailVertCount;
			detailTriCount = H->detailTriCount;
			bvNodeCount = H->bvNodeCount;
			offMeshConCount = H->offMeshConCount;
			offMeshSegConCount = H->offMeshSegConCount;
			clusterCount = H->clusterCount;
		}

		Ar << totVertCount << totPolyCount << maxLinkCount;
		Ar << detailMeshCount << detailVertCount << detailTriCount;
		Ar << bvNodeCount << offMeshConCount << offMeshSegConCount;
		Ar << clusterCount;
		int32 polyClusterCount = detailMeshCount;

		// calc sizes for our data so we know how much to allocate and where to read/write stuff
		// note this may not match the on-disk size or the in-memory size on the machine that generated that data
		const int32 headerSize = dtAlign4(sizeof(dtMeshHeader));
		const int32 vertsSize = dtAlign4(sizeof(float) * 3 * totVertCount);
		const int32 polysSize = dtAlign4(sizeof(dtPoly) * totPolyCount);
		const int32 linksSize = dtAlign4(sizeof(dtLink) * maxLinkCount);
		const int32 detailMeshesSize = dtAlign4(sizeof(dtPolyDetail) * detailMeshCount);
		const int32 detailVertsSize = dtAlign4(sizeof(float) * 3 * detailVertCount);
		const int32 detailTrisSize = dtAlign4(sizeof(unsigned char) * 4 * detailTriCount);
		const int32 bvTreeSize = dtAlign4(sizeof(dtBVNode) * bvNodeCount);
		const int32 offMeshConsSize = dtAlign4(sizeof(dtOffMeshConnection) * offMeshConCount);
		const int32 offMeshSegsSize = dtAlign4(sizeof(dtOffMeshSegmentConnection) * offMeshSegConCount);
		const int32 clusterSize = dtAlign4(sizeof(dtCluster) * clusterCount);
		const int32 polyClustersSize = dtAlign4(sizeof(unsigned short) * polyClusterCount);

		if (true == Ar.hasModeFlag(SerializerMode::Read))
		{
			TA_ASSERT_DEV((nullptr == TileData), "비정상");

			// allocate data chunk for this navmesh tile.  this is its final destination.
			TileDataSize = headerSize + vertsSize + polysSize + linksSize +
				detailMeshesSize + detailVertsSize + detailTrisSize +
				bvTreeSize + offMeshConsSize + offMeshSegsSize +
				clusterSize + polyClustersSize;
			TileData = (unsigned char*)dtAlloc(sizeof(unsigned char) * TileDataSize, DT_ALLOC_PERM);
			if (!TileData)
			{
				TA_ASSERT_DEV((nullptr == TileData), "비정상");
			}
			memset(TileData, 0, TileDataSize);
		}
		else if (true == Ar.hasModeFlag(SerializerMode::Write))
		{
			// TileData and TileDataSize should already be set, verify
			TA_ASSERT_DEV((nullptr != TileData), "비정상");
		}

		if (TileData != NULL)
		{
			// sort out where various data types do/will live
			unsigned char* d = TileData;
			dtMeshHeader* Header = (dtMeshHeader*)d; d += headerSize;
			float* NavVerts = (float*)d; d += vertsSize;
			dtPoly* NavPolys = (dtPoly*)d; d += polysSize;
			d += linksSize;			// @fixme, are links autogenerated on addTile?
			dtPolyDetail* DetailMeshes = (dtPolyDetail*)d; d += detailMeshesSize;
			float* DetailVerts = (float*)d; d += detailVertsSize;
			unsigned char* DetailTris = (unsigned char*)d; d += detailTrisSize;
			dtBVNode* BVTree = (dtBVNode*)d; d += bvTreeSize;
			dtOffMeshConnection* OffMeshCons = (dtOffMeshConnection*)d; d += offMeshConsSize;
			dtOffMeshSegmentConnection* OffMeshSegs = (dtOffMeshSegmentConnection*)d; d += offMeshSegsSize;
			dtCluster* Clusters = (dtCluster*)d; d += clusterSize;
			unsigned short* PolyClusters = (unsigned short*)d; d += polyClustersSize;

			TA_ASSERT_DEV((d == (TileData + TileDataSize)), "비정상");

			// now serialize the data in the blob!

			// header
			Ar << Header->magic << Header->version << Header->x << Header->y;
			Ar << Header->layer << Header->userId << Header->polyCount << Header->vertCount;
			Ar << Header->maxLinkCount << Header->detailMeshCount << Header->detailVertCount << Header->detailTriCount;
			Ar << Header->bvNodeCount << Header->offMeshConCount << Header->offMeshBase;
			Ar << Header->walkableHeight << Header->walkableRadius << Header->walkableClimb;
			Ar << Header->bmin[0] << Header->bmin[1] << Header->bmin[2];
			Ar << Header->bmax[0] << Header->bmax[1] << Header->bmax[2];
			Ar << Header->bvQuantFactor;
			Ar << Header->clusterCount;
			Ar << Header->offMeshSegConCount << Header->offMeshSegPolyBase << Header->offMeshSegVertBase;

			// mesh and offmesh connection vertices, just an array of floats (one float triplet per vert)
			{
				float* F = NavVerts;
				for (int32 VertIdx = 0; VertIdx < totVertCount; VertIdx++)
				{
					Ar << *F; F++;
					Ar << *F; F++;
					Ar << *F; F++;
				}
			}

			// mesh and off-mesh connection polys
			for (int32 PolyIdx = 0; PolyIdx < totPolyCount; ++PolyIdx)
			{
				dtPoly& P = NavPolys[PolyIdx];
				Ar << P.firstLink;

				for (uint32 VertIdx = 0; VertIdx < DT_VERTS_PER_POLYGON; ++VertIdx)
				{
					Ar << P.verts[VertIdx];
				}
				for (uint32 NeiIdx = 0; NeiIdx < DT_VERTS_PER_POLYGON; ++NeiIdx)
				{
					Ar << P.neis[NeiIdx];
				}
				Ar << P.flags << P.vertCount << P.areaAndtype;
			}

			// serialize detail meshes
			for (int32 MeshIdx = 0; MeshIdx < detailMeshCount; ++MeshIdx)
			{
				dtPolyDetail& DM = DetailMeshes[MeshIdx];
				Ar << DM.vertBase << DM.triBase << DM.vertCount << DM.triCount;
			}

			// serialize detail verts (one float triplet per vert)
			{
				float* F = DetailVerts;
				for (int32 VertIdx = 0; VertIdx < detailVertCount; ++VertIdx)
				{
					Ar << *F; F++;
					Ar << *F; F++;
					Ar << *F; F++;
				}
			}

			// serialize detail tris (4 one-byte indices per tri)
			{
				unsigned char* V = DetailTris;
				for (int32 TriIdx = 0; TriIdx < detailTriCount; ++TriIdx)
				{
					Ar << *V; V++;
					Ar << *V; V++;
					Ar << *V; V++;
					Ar << *V; V++;
				}
			}

			// serialize BV tree
			for (int32 NodeIdx = 0; NodeIdx < bvNodeCount; ++NodeIdx)
			{
				dtBVNode& Node = BVTree[NodeIdx];
				Ar << Node.bmin[0] << Node.bmin[1] << Node.bmin[2];
				Ar << Node.bmax[0] << Node.bmax[1] << Node.bmax[2];
				Ar << Node.i;
			}

			// serialize off-mesh connections
			for (int32 ConnIdx = 0; ConnIdx < offMeshConCount; ++ConnIdx)
			{
				dtOffMeshConnection& Conn = OffMeshCons[ConnIdx];
				Ar << Conn.pos[0] << Conn.pos[1] << Conn.pos[2] << Conn.pos[3] << Conn.pos[4] << Conn.pos[5];
				Ar << Conn.rad << Conn.poly << Conn.flags << Conn.side << Conn.userId;
			}

			//if (NavMeshVersion >= NAVMESHVER_OFFMESH_HEIGHT_BUG)
			{
				for (int32 ConnIdx = 0; ConnIdx < offMeshConCount; ++ConnIdx)
				{
					dtOffMeshConnection& Conn = OffMeshCons[ConnIdx];
					Ar << Conn.height;
				}
			}

			for (int32 SegIdx = 0; SegIdx < offMeshSegConCount; ++SegIdx)
			{
				dtOffMeshSegmentConnection& Seg = OffMeshSegs[SegIdx];
				Ar << Seg.startA[0] << Seg.startA[1] << Seg.startA[2];
				Ar << Seg.startB[0] << Seg.startB[1] << Seg.startB[2];
				Ar << Seg.endA[0] << Seg.endA[1] << Seg.endA[2];
				Ar << Seg.endB[0] << Seg.endB[1] << Seg.endB[2];
				Ar << Seg.rad << Seg.firstPoly << Seg.npolys << Seg.flags << Seg.userId;
			}

			// serialize clusters
			for (int32 CIdx = 0; CIdx < clusterCount; ++CIdx)
			{
				dtCluster& cluster = Clusters[CIdx];
				Ar << cluster.center[0] << cluster.center[1] << cluster.center[2];
			}

			// serialize poly clusters map
			{
				unsigned short* C = PolyClusters;
				for (int32 PolyClusterIdx = 0; PolyClusterIdx < polyClusterCount; ++PolyClusterIdx)
				{
					Ar << *C; C++;
				}
			}
		}
	}

	void CommonMoveActorSystem::serializeCompressedTileCacheData(Serializer& Ar, int32 NavMeshVersion, unsigned char*& CompressedData, int32& CompressedDataSize) noexcept
	{
		//Ar << CompressedDataSize;

		//if (CompressedDataSize > 0)
		//{
		//	if (Ar.IsLoading())
		//	{
		//		CompressedData = (unsigned char*)dtAlloc(sizeof(unsigned char) * CompressedDataSize, DT_ALLOC_PERM);
		//		if (!CompressedData)
		//		{
		//			UE_LOG(LogNavigation, Error, TEXT("Failed to alloc tile compressed data"));
		//		}
		//		FMemory::Memset(CompressedData, 0, CompressedDataSize);
		//	}

		//	Ar.Serialize(CompressedData, CompressedDataSize);
		//}
	}
#endif

	bool CommonMoveActorSystem::processMoveActor(CommonActor* target, const Vector& newPos, const bool isForced) const noexcept
	{
		return true;
	}

	bool CommonMoveActorSystem::processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept
	{
		return true;
	}

	bool CommonMoveActorSystem::processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept
	{
		return true;
	}

	CommonMoveActorSystem::CommonMoveActorSystem(void) noexcept
		: ActorSystem(ActorSystemType::MoveSystem)
	{
	}

	bool CommonMoveActorSystem::moveActor(CommonActor* target
							  , const Vector& newPos
							  , const bool isForced /*= false*/
							  , std::unordered_set<SectorKey>* oldSectors /*= nullptr*/
							  , std::unordered_set<SectorKey>* newSectors /*= nullptr*/) const noexcept
	{
		CommonMoveActorComponent* actorMoveCom = GetActorComponent<CommonMoveActorComponent>(target);
		if (nullptr == actorMoveCom)
		{
			TA_ASSERT_DEV(false, "컴포넌트가 없습니다.")
			return false;
		}

		const SectorKey newSectorKey = GetSectorIndex(newPos);
		// 유효한 섹터에 위치하고 있지않다. 그냥 리턴하자
		if (false == newSectorKey.isValid())
		{
			TA_ASSERT_DEV(false, "섹터안에 있지 않습니다.")
			return false;
		}

		{
			ScopedLock componentLock(actorMoveCom);

			if (false == move_(actorMoveCom, newPos, isForced))
			{
				TA_ASSERT_DEV(false, "move 단계에서 실패했습니다.")
					return false;
			}

			std::unordered_set<SectorKey>& nearSectors = actorMoveCom->getNewNearSectors_();
			const SectorKey currentSectorKey = actorMoveCom->getCurrentSectorKey_();
			if (newSectorKey != currentSectorKey)
			{
				if (false == moveSector_(actorMoveCom, newSectorKey))
				{
					TA_ASSERT_DEV(false, "moveSector 단계에서 실패했습니다.")
						return false;
				}

				if (nullptr != oldSectors)
				{
					*oldSectors = nearSectors;
				}
				
				GetNearSectors(newSectorKey, nearSectors);

				if (nullptr != newSectors)
				{
					*newSectors = nearSectors;
				}
			}
		}

		return true;
	}

	ActorKey CommonMoveActorSystem::getTargetActorFromSector(CommonMoveActorComponent* myMove
															 , const ActorType& actorType
															 , const bool searchNearSectors) const noexcept
	{
		std::vector<ActorType> actorTypes;
		actorTypes.reserve(1);
		actorTypes.push_back(actorType);

		return getTargetActorFromSector(myMove, actorTypes, searchNearSectors);
	}

	ActorKey CommonMoveActorSystem::getTargetActorFromSector(CommonMoveActorComponent* myMove
															 , const std::vector<ActorType>& actorTypes
															 , const bool searchNearSectors) const noexcept
	{
		Sector* sector = nullptr;
		ActorKey targetActorKey;
		if (false == searchNearSectors)
		{
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				sectorKey = myMove->getCurrentSectorKey_();
			}

			sector = GetSector(sectorKey);
			if (nullptr == sector)
			{
				TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
				return ActorKey();
			}

			targetActorKey = sector->getTargetActorFromSector(actorTypes);
		}
		else
		{
			std::unordered_set<SectorKey> nearSectors;
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				nearSectors = myMove->getNewNearSectors_();
				sectorKey = myMove->getCurrentSectorKey_();
			}

			std::unordered_set<SectorKey>::iterator it = nearSectors.begin();
			const std::unordered_set<SectorKey>::iterator end = nearSectors.end();
			for (; it != end; ++it)
			{
				sector = GetSector(sectorKey);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
					continue;
				}

				targetActorKey = sector->getTargetActorFromSector(actorTypes);
				if (true == targetActorKey.isValid())
				{
					break;
				}
			}
		}

		return targetActorKey;
	}

	void CommonMoveActorSystem::getTargetActorsFromSector(CommonMoveActorComponent* myMove
														  , const std::vector<ActorType>& actorTypes
														  , std::vector<ActorKey>& output
														  , const bool searchNearSectors) const noexcept
	{
		output.clear();
		Sector* sector = nullptr;
		if (false == searchNearSectors)
		{
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				sectorKey = myMove->getCurrentSectorKey_();
			}

			sector = GetSector(sectorKey);
			if (nullptr == sector)
			{
				TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
			}

			sector->getTargetActorsFromSector(actorTypes, output);
		}
		else
		{
			std::unordered_set<SectorKey> nearSectors;
			SectorKey sectorKey;
			{
				ScopedLock componentLock(myMove);
				nearSectors = myMove->getNewNearSectors_();
				sectorKey = myMove->getCurrentSectorKey_();
			}

			std::unordered_set<SectorKey>::iterator it = nearSectors.begin();
			const std::unordered_set<SectorKey>::iterator end = nearSectors.end();
			for (; it != end; ++it)
			{
				sector = GetSector(sectorKey);
				if (nullptr == sector)
				{
					TA_ASSERT_DEV(false, "섹터가 비정상입니다.");
					continue;
				}

				sector->getTargetActorsFromSector(actorTypes, output);
			}
		}
	}

	bool CommonMoveActorSystem::move_(CommonMoveActorComponent* target
									  , const Vector& newPos
									  , const bool isForced) const noexcept
	{
		if (false == isForced && false == checkCanMove_(target, newPos))
		{
			return false;
		}

		if (false == target->setCurrentPosition_(newPos))
		{
			return false;
		}

		return true;
	}

	bool CommonMoveActorSystem::moveSector_(CommonMoveActorComponent* target, const SectorKey& newSectorKey) const noexcept
	{
		SectorKey currentSectorKey = target->getCurrentSectorKey_();
		if (newSectorKey == currentSectorKey)
		{
			return false;
		}

		CommonActor* owner = target->getOwner();
		Sector* newSector = GetSector(newSectorKey);
		TA_ASSERT_DEV((nullptr != newSector), "코드버그입니다..");

		// 처음에 아무곳도 안갔을때..
		if (true == currentSectorKey.isValid())
		{
			Sector* oldSector = GetSector(currentSectorKey);
			TA_ASSERT_DEV((nullptr != oldSector), "코드버그입니다..")
			oldSector->exitSector(owner);
		}

		newSector->enterSector(owner);
		target->setCurrentSectorKey_(newSectorKey);
		
		return true;
	}

	bool CommonMoveActorSystem::checkCanMove_(CommonMoveActorComponent* moveCom, const Vector& newPos) const noexcept
	{
		const float distance = (moveCom->getCurrentPosition_() - newPos).sizeSquared();
		// 나중에 스피드 검증이랑 추가로 들어가야한다
		if ((MaxMoveDistance* MaxMoveDistance) < distance)
		{
			return false;
		}

		return true;
	}

}

