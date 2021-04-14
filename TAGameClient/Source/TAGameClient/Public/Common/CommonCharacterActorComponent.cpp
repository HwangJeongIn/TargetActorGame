#include "Common/CommonCharacterActorComponent.h"
#include "Common/ComponentData.h"
#include "Common/GetComponentAndSystem.h"

namespace ta
{
	CommonCharacterActorComponent::~CommonCharacterActorComponent(void) noexcept
	{
	}

	bool CommonCharacterActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		const CommonCharacterComponentData* data = static_cast<const CommonCharacterComponentData*>(componentData);

		_characterGameData = GetGameData<CharacterGameData>(data->_characterGameDataKey);
		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_currentHp = data->_currentHp;

		return true;
	}

	ComponentData* CommonCharacterActorComponent::packData_(void) noexcept
	{
		CommonCharacterComponentData* data = new CommonCharacterComponentData;

		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return nullptr;
		}

		data->_characterGameDataKey = _characterGameData->_key;
		data->_currentHp			= _currentHp;

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

	const CharacterGameData* CommonCharacterActorComponent::getCharacterGameData_(void) const noexcept
	{
		return _characterGameData;
	}

	const RenderingGameData* CommonCharacterActorComponent::getRenderingGameData_(void) const noexcept
	{
		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "character 게임데이터가 없습니다.");
			return nullptr;
		}

		if (false == _characterGameData->_renderingGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상적인 RenderingGameDataKey 입니다.");
			return nullptr;
		}

		const ta::RenderingGameData* renderingGameData = ta::GetGameData<ta::RenderingGameData>(_characterGameData->_renderingGameDataKey);
		if (nullptr == renderingGameData)
		{
			TA_ASSERT_DEV(false, "RenderingGameData가 없습니다.");
			return nullptr;
		}

		return renderingGameData;
	}

	float CommonCharacterActorComponent::getCurrentHp_(void) const noexcept
	{
		return _currentHp;
	}

	bool CommonCharacterActorComponent::setCurrentHp_(const float value) noexcept
	{
		if ((nullptr == _characterGameData)
			|| (_characterGameData->_maxHp < value))
		{
			return false;
		}

		if (0.0f > value)
		{
			_currentHp = 0.0f;
		}
		else
		{
			_currentHp = value;
		}

		return true;
	}
}

