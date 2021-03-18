// Copyright Epic Games, Inc. All Rights Reserved.

#include "RecastNavigation/RecastNavigationSystemInclude.h"
#include "RecastNavigation/DetourCrowd/DetourSharedBoundary.h"

namespace ta
{
	void dtSharedBoundary::Initialize()
	{
		CurrentTime = 0.0f;
		NextClearTime = 0.0f;

		for (int32 Idx = 0; Idx < DT_MAX_AREAS; Idx++)
		{
			SingleAreaFilter.setAreaCost(Idx, DT_UNWALKABLE_POLY_COST);
		}
	}

	void dtSharedBoundary::Tick(float DeltaTime)
	{
		CurrentTime += DeltaTime;

		// clear unused entries
		if (CurrentTime > NextClearTime)
		{
			const float MaxLifeTime = 2.0f;
			NextClearTime = CurrentTime + MaxLifeTime;


			std::vector<dtSharedBoundaryData>::iterator It = Data.begin();
			//const std::vector<dtSharedBoundaryData>::const_iterator End = Data.end();
			for (; It != Data.end();)
			{
				const float LastAccess = CurrentTime - It->AccessTime;
				if (LastAccess >= MaxLifeTime)
				{
					It = Data.erase(It);
				}
				else
				{
					++It;
				}
			}
		}
	}

	int32 dtSharedBoundary::CacheData(float* Center, float Radius, dtPolyRef CenterPoly, dtNavMeshQuery* NavQuery, dtQueryFilter* NavFilter)
	{
		// bail if requested poly is not valid (e.g. rebuild in progress)
		if (NavQuery && !NavQuery->isValidPolyRef(CenterPoly, NavFilter))
		{
			return -1;
		}

		Radius *= 1.5f;

		int32 DataIdx = FindData(Center, Radius, CenterPoly, NavFilter);
		const bool bHasValidData = IsValid(DataIdx, NavQuery, NavFilter);
		if (!bHasValidData)
		{
			if (DataIdx >= 0)
			{
				// remove in next cleanup
				Data[DataIdx].AccessTime = 0.0f;
			}

			dtSharedBoundaryData NewData;
			dtVcopy(NewData.Center, Center);
			NewData.Radius = Radius;
			NewData.Filter = NavFilter;
			NewData.SingleAreaId = 0;

			FindEdges(NewData, CenterPoly, NavQuery, NavFilter);
			Data.push_back(NewData);
			DataIdx = Data.size() - 1;
		}

		Data[DataIdx].AccessTime = CurrentTime;
		return DataIdx;
	}

	int32 dtSharedBoundary::CacheData(float* Center, float Radius, dtPolyRef CenterPoly, dtNavMeshQuery* NavQuery, uint8 SingleAreaId)
	{
		SingleAreaFilter.setAreaCost(SingleAreaId, 1.0f);

		// bail if requested poly is not valid (e.g. rebuild in progress)
		if (NavQuery && !NavQuery->isValidPolyRef(CenterPoly, &SingleAreaFilter))
		{
			return -1;
		}

		Radius *= 1.5f;

		int32 DataIdx = FindData(Center, Radius, CenterPoly, SingleAreaId);
		const bool bHasValidData = IsValid(DataIdx, NavQuery, &SingleAreaFilter);
		if (!bHasValidData)
		{
			if (DataIdx >= 0)
			{
				// remove in next cleanup
				Data[DataIdx].AccessTime = 0.0f;
			}

			dtSharedBoundaryData NewData;
			dtVcopy(NewData.Center, Center);
			NewData.Radius = Radius;
			NewData.SingleAreaId = SingleAreaId;

			FindEdges(NewData, CenterPoly, NavQuery, &SingleAreaFilter);
			Data.push_back(NewData);
			DataIdx = Data.size() - 1;
		}

		SingleAreaFilter.setAreaCost(SingleAreaId, DT_UNWALKABLE_POLY_COST);
		Data[DataIdx].AccessTime = CurrentTime;
		return DataIdx;
	}

	void dtSharedBoundary::FindEdges(dtSharedBoundaryData& SharedData, dtPolyRef CenterPoly, dtNavMeshQuery* NavQuery, dtQueryFilter* NavFilter)
	{
		const int32 MaxWalls = 64;
		int32 NumWalls = 0;
		float WallSegments[MaxWalls * 3 * 2] = { 0 };
		dtPolyRef WallPolys[MaxWalls * 2] = { 0 };

		const int32 MaxNeis = 64;
		int32 NumNeis = 0;
		dtPolyRef NeiPolys[MaxNeis] = { 0 };

		NavQuery->findWallsInNeighbourhood(CenterPoly, SharedData.Center, SharedData.Radius, NavFilter,
										   NeiPolys, &NumNeis, MaxNeis, WallSegments, WallPolys, &NumWalls, MaxWalls);

		dtSharedBoundaryEdge NewEdge;
		for (int32 Idx = 0; Idx < NumWalls; Idx++)
		{
			dtVcopy(NewEdge.v0, &WallSegments[Idx * 6]);
			dtVcopy(NewEdge.v1, &WallSegments[Idx * 6 + 3]);
			NewEdge.p0 = WallPolys[Idx * 2];
			NewEdge.p1 = WallPolys[Idx * 2 + 1];

			SharedData.Edges.push_back(NewEdge);
		}

		SharedData.Polys.reserve(NumNeis);
		for (int32 Idx = 0; Idx < NumNeis; Idx++)
		{
			SharedData.Polys.insert(NeiPolys[Idx]);
		}
	}

	int32 dtSharedBoundary::FindData(float* Center, float Radius, dtPolyRef ReqPoly, dtQueryFilter* NavFilter) const
	{
		const float RadiusThr = 50.0f;

		float TempValue = (Radius * 0.5f);
		const float DistThrSq = TempValue * TempValue;

		const int32 DataSize = Data.size();
		for (int32 Idx = 0; Idx < DataSize; Idx++)
		{
			if (Data[Idx].Filter == NavFilter)
			{
				const float DistSq = dtVdistSqr(Center, Data[Idx].Center);
				if (DistSq <= DistThrSq && dtAbs(Data[Idx].Radius - Radius) < RadiusThr)
				{
					if (0 < Data[Idx].Polys.count(ReqPoly))
					{
						return Idx;
					}
				}
			}
		}

		return -1;
	}

	int32 dtSharedBoundary::FindData(float* Center, float Radius, dtPolyRef ReqPoly, uint8 SingleAreaId) const
	{
		float TempValue = (Radius * 0.5f);
		const float DistThrSq = TempValue * TempValue;
		const float RadiusThr = 50.0f;

		const int32 DataSize = Data.size();
		for (int32 Idx = 0; Idx < DataSize; Idx++)
		{
			if (Data[Idx].SingleAreaId == SingleAreaId)
			{
				const float DistSq = dtVdistSqr(Center, Data[Idx].Center);
				if (DistSq <= DistThrSq && dtAbs(Data[Idx].Radius - Radius) < RadiusThr)
				{
					if (0 < Data[Idx].Polys.count(ReqPoly))
					{
						return Idx;
					}
				}
			}
		}

		return -1;
	}

	bool dtSharedBoundary::HasSample(int32 Idx) const
	{
		return (Idx >= 0) && (Idx < Data.size());//&& Data.IsAllocated(Idx);
	}

	bool dtSharedBoundary::IsValid(int32 Idx, dtNavMeshQuery* NavQuery, dtQueryFilter* NavFilter) const
	{
		bool bValid = HasSample(Idx);
		if (bValid)
		{
			std::unordered_set<dtPolyRef>::const_iterator It = Data[Idx].Polys.begin();
			const std::unordered_set<dtPolyRef>::const_iterator End = Data[Idx].Polys.end();
			for (; It != End; ++It)
			{
				const dtPolyRef TestRef = *It;
				const bool bValidRef = NavQuery->isValidPolyRef(TestRef, NavFilter);
				if (!bValidRef)
				{
					bValid = false;
					break;
				}
			}
		}

		return bValid;
	}
}
