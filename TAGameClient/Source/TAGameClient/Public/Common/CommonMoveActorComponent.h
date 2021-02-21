#pragma once

#include "Common/ActorComponent.h"
#include "Common/Vector.h"
#include "Common/KeyDefinition.h"
#include <unordered_set>


namespace ta
{
	class CommonMoveActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonMoveActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;
		virtual ComponentData* packData_(void) noexcept override;

		virtual void onActive(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;

		std::unordered_set<SectorKey>& getNewNearSectors_(void) noexcept;
		const std::unordered_set<SectorKey>& getNewNearSectors_(void) const noexcept;
		bool setNewNearSectors_(const std::unordered_set<SectorKey>& newNearSectors) noexcept;

		const Vector& getCurrentPosition_(void) const noexcept;
		bool setCurrentPosition_(const Vector& input) noexcept;

		const Vector& getCurrentRotation_(void) const noexcept;
		bool setCurrentRotation_(const Vector& input) noexcept;

		float getSpeed_(void) const noexcept;
		bool setSpeed_(float input) noexcept;

		SectorKey getCurrentSectorKey_(void) const noexcept;
		bool setCurrentSectorKey_(const SectorKey& input) noexcept;


	protected:
		CommonMoveActorComponent(void) noexcept;

	private:
		Vector _currentPosition;
		Vector _currentRotation;
		float _speed;

		SectorKey _currentSectorKey;
		std::unordered_set<SectorKey> _nearSectors;
	};
}
