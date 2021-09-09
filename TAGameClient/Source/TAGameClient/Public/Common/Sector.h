#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Lockable.h"
#include "Common/Uncopyable.h"
#include <unordered_set>



#define GetNearSector_v1
//#define GetNearSector_v2

namespace ta
{
	class CommonActor;
	class Vector;
	class Sector;
	class SectorProcessor;
	class ContentParameter;
}


namespace ta
{
	constexpr float SectorSize = 3000.0f;
	constexpr float SectorHalfSize = SectorSize / 2;
	constexpr uint32 CountOfOneSideSectors = 1000;
	constexpr uint32 HalfCountOfOneSideSectors = (CountOfOneSideSectors / 2);
	constexpr uint32 CountOfSectors = CountOfOneSideSectors * CountOfOneSideSectors;
	constexpr float MaxPositionValue = SectorSize * (CountOfOneSideSectors / 2);
	constexpr float MaxPositionValueExcludingSideSector = MaxPositionValue - SectorSize;
	/*

		+x

	-y	 O	+y

		-x


	x : LT			x : RT
	

			x Origin


	x : LD			x : RD
	
	< (X,Y) 좌표 >

	Origin = ( 0, 0 )
	LT = ( + SectorSize * (CountOfOneSideSectors / 2), - SectorSize * (CountOfOneSideSectors / 2) )
	RT = ( + SectorSize * (CountOfOneSideSectors / 2), + SectorSize * (CountOfOneSideSectors / 2) )
	LD = ( - SectorSize * (CountOfOneSideSectors / 2), - SectorSize * (CountOfOneSideSectors / 2) )
	RD = ( - SectorSize * (CountOfOneSideSectors / 2), + SectorSize * (CountOfOneSideSectors / 2) )
	*/
}

namespace ta
{
	extern Sector* GetSector(const SectorKey& sectorKey) noexcept;
	extern bool CheckIsInSector(const Vector& position) noexcept;
	extern SectorKey GetSectorIndex(const Vector& position) noexcept;

	extern bool GetSectorIndexXY(const SectorKey& sectorKey, int32& indexX, int32& indexY) noexcept;
	extern SectorKey GetSectorKey(const int32& indexX, const int32& indexY) noexcept;
	// Sector의 멤버로하면 외부해서 필요할때마다 Sector객체 가져와서 사용해야해서 그냥 바로 사용할 수 있게 전역으로 했다
	// 이 함수들은 Sector클래스 정보없이 계산 된다.
	extern bool GetSectorCenterPosition(const SectorKey& sectorKey, Vector& output) noexcept;
	extern bool GetSectorRandomPosition(const SectorKey& sectorKey, Vector& output) noexcept;

	// 필요해지면 만들자 현재 MoveActorComponent에 캐싱하고 있어서 따로 필요없을것 같음
	//extern bool GetNearSectors(const SectorKey& sectorKey, std::vector<SectorKey>& output) noexcept;

	// 섹터 이동할때 기존에 있던 섹터와 새로 이동한 섹터사이의 near sector diff 하기 위해서 검색 빠른 set 반환한다
	extern bool GetNearSectors(const SectorKey& sectorKey, std::unordered_set<SectorKey>& output) noexcept;
	extern bool GetNearSectors(const SectorKey& sectorKey, std::unordered_set<SectorKey>& output) noexcept;
	extern void CompareAndGetDifferentSectors(const std::unordered_set<SectorKey>& fromSectors
											  , const std::unordered_set<SectorKey>& toSectors
											  , std::vector<SectorKey>& addedSectors
											  , std::vector<SectorKey>& removedSectors) noexcept;
}


namespace ta
{
	class Sector : public Uncopyable, public Lockable
	{
		//: 액터키리스트
		//	섹터 이동 함수
		//	= > 섹터 이동시, 이전 섹터에서 나 삭제하고, 새로운 섹터에 나 추가한다.액터의 주변 섹터 변경
		//	추가된 섹터 : 나한테 해당 섹터 액터들 클라로 보내고 해당 섹터 액터들에게 나 각 클라로 보내기 // 클라섹터 갱신 > 해당클라에서 액터 추가
		//	삭제된 섹터 : 나한테 해당 섹터 액터들 삭제하도록 클라로 보내고 해당 섹터 액터들에게 나 삭제하도록 클라로 보내기 // 클라섹터 갱신 > 해당클라에서 액터 삭제
	public:
		Sector(void) noexcept;
		virtual ~Sector(void) noexcept;

		virtual bool initialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;

		void setSectorKey(const SectorKey& sectorKey) noexcept;
		bool enterSector(CommonActor* actor) noexcept;
		bool exitSector(CommonActor* actor) noexcept;

		const std::unordered_set<ActorKey>& getActors_(void) const noexcept;
		ActorKey getTargetActorFromSector(const std::vector<ActorType>& actorTypes) noexcept;

		void getTargetActorsFromSector(const ActorType& actorType, std::unordered_set<ActorKey>& output) noexcept;
		void getTargetActorsFromSector(const std::vector<ActorType>& actorTypes, std::unordered_set<ActorKey>& output) noexcept;

		void getTargetActorsFromSector(const ActorType& actorType, std::vector<ActorKey>& output) noexcept;
		void getTargetActorsFromSector(const std::vector<ActorType>& actorTypes, std::vector<ActorKey>& output) noexcept;


		void getTargetActorsAndOtherActorsFromSector(const ActorType& actorType
													 , std::unordered_set<ActorKey>& typeActors
													 , std::unordered_set<ActorKey>& otherTypeActors) noexcept;

		void getTargetActorsAndOtherActorsFromSector(const std::vector<ActorType>& actorTypes
													 , std::unordered_set<ActorKey>& typeActors
													 , std::unordered_set<ActorKey>& otherTypeActors) noexcept;


		void getTargetActorsWithActorTypeFromSector(std::unordered_map<ActorKey, ActorType>& output) noexcept;


		virtual uint32 getOwnedActorCountForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept;
		virtual bool checkOwnedActorExistenceForServer_(const GroupGameDataKey& groupGameDataKey) const noexcept;
		virtual bool registerToOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept;
		virtual bool deregisterFromOwnedActorsForServer_(const GroupGameDataKey& groupGameDataKey, const ActorKey& actorKey) noexcept;

		virtual bool startSectorEventForServer(const ContentParameter& parameter, uint32 sectorEventIndex, bool isBasicSectorEvent) noexcept;

	private:
		bool removeActor_(const ActorKey& actor) noexcept;
		bool addActor_(const ActorKey& actor) noexcept;

	protected:
		SectorKey _sectorKey;
		// 성능 테스트 결과 vector보다 unordered_set가 더 좋다
		std::unordered_set<ActorKey> _actors;

	};
}