#include "Common/CommonCharacterActorComponent.h"
#include "Common/ComponentData.h"

namespace ta
{
	CommonCharacterActorComponent::~CommonCharacterActorComponent(void) noexcept
	{
	}

	bool CommonCharacterActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		const CommonCharacterComponentData* data = static_cast<const CommonCharacterComponentData*>(componentData);

		_strength = data->_strength;
		_agility = data->_agility;
		_maxHp = data->_maxHp;
		_currentHp = data->_currentHp;

		return true;
	}

	ComponentData* CommonCharacterActorComponent::packData_(void) noexcept
	{
		CommonCharacterComponentData* data = new CommonCharacterComponentData;

		data->_strength		= _strength;
		data->_agility		= _agility;
		data->_maxHp		= _maxHp;
		data->_currentHp	= _currentHp;

		return data;
	}

	const ActorComponentType CommonCharacterActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Character;
	}

	CommonCharacterActorComponent::CommonCharacterActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Character)
	{
	}
}

