#include "Common/CommonMoveActorComponent.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/CommonMoveActorSystem.h"
#include "Common/ScopedLock.h"


namespace ta
{
	CommonMoveActorComponent::~CommonMoveActorComponent(void) noexcept
	{
	}

	bool CommonMoveActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		// 이런경우(데이터를 이용해서 이동)도 있지만 보통 내부 데이터를 초기화한다.
		const CommonMoveComponentData* data = static_cast<const CommonMoveComponentData*>(componentData);
		
		{
			ScopedLock lock(this);
			setCurrentPosition_(data->_position);
			setCurrentRotation_(data->_rotation);
			setSpeed_(data->_speed);
		}
		

		return true;
	}

	ComponentData* CommonMoveActorComponent::packData_(void) noexcept
	{
		CommonMoveComponentData* data = new CommonMoveComponentData;

		// Common초기화
		data->_position = _currentPosition;
		data->_rotation = _currentRotation;
		data->_speed = _speed;

		return data;
	}

	void CommonMoveActorComponent::onActive(void) noexcept
	{
	}

	const ActorComponentType CommonMoveActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Move;
	}

	std::unordered_set<SectorKey>& CommonMoveActorComponent::getNewNearSectors_(void) noexcept
	{
		return _nearSectors;
	}

	const std::unordered_set<SectorKey>& CommonMoveActorComponent::getNewNearSectors_(void) const noexcept
	{
		return _nearSectors;
	}

	bool CommonMoveActorComponent::setNewNearSectors_(const std::unordered_set<SectorKey>& newNearSectors) noexcept
	{
		_nearSectors = newNearSectors;
		return true;
	}

	const Vector& CommonMoveActorComponent::getCurrentPosition_(void) const noexcept
	{
		return _currentPosition;
	}

	bool CommonMoveActorComponent::setCurrentPosition_(const Vector& input) noexcept
	{
		_currentPosition = input;
		return true;
	}

	const Vector& CommonMoveActorComponent::getCurrentRotation_(void) const noexcept
	{
		return _currentRotation;
	}

	bool CommonMoveActorComponent::setCurrentRotation_(const Vector& input) noexcept
	{
		_currentRotation = input;
		return true;
	}

	float CommonMoveActorComponent::getSpeed_(void) const noexcept
	{
		return _speed;
	}

	bool CommonMoveActorComponent::setSpeed_(float input) noexcept
	{
		_speed = input;
		return true;
	}

	SectorKey CommonMoveActorComponent::getCurrentSectorKey_(void) const noexcept
	{
		return _currentSectorKey;
	}

	bool CommonMoveActorComponent::setCurrentSectorKey_(const SectorKey& input) noexcept
	{
		_currentSectorKey = input;
		return true;
	}

	CommonMoveActorComponent::CommonMoveActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Move)
	{
	}
}

