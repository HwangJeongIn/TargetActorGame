#include "Common/CommonCharacterActorComponent.h"
#include "Common/ComponentData.h"
#include "Common/GetComponentAndSystem.h"
#include "Common/BuffObject.h"


namespace ta
{
	CommonCharacterActorComponent::~CommonCharacterActorComponent(void) noexcept
	{
	}

	bool CommonCharacterActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		const CommonCharacterComponentData* data = static_cast<const CommonCharacterComponentData*>(componentData);

		if (false == setCharacterGameData_(data->_characterGameDataKey))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

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

		const uint8 count = static_cast<uint8>(CharacterStatType::Count);
		for (uint8 index = 0; index < count; ++index)
		{
			data->_currentStats[index] = _currentStats[index];
		}

		return data;
	}

	const ActorComponentType CommonCharacterActorComponent::getActorComponentType(void) noexcept
	{
		return ActorComponentType::Character;
	}

	CommonCharacterActorComponent::CommonCharacterActorComponent(void) noexcept
		: ActorComponent(ActorComponentType::Character)
	{
		const uint8 count = static_cast<uint8>(CharacterStatType::Count);
		for (uint8 index = 0; index < count; ++index)
		{
			_currentStats[index] = 0.0f;
		}
	}

	bool CommonCharacterActorComponent::setCharacterGameData_(const CharacterGameDataKey& key) noexcept
	{
		_characterGameData = GetGameData<CharacterGameData>(key);
		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		onChangedCharacterGameData();

		return true;
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

		if (nullptr == _characterGameData->_renderingGameData)
		{
			TA_ASSERT_DEV(false, "비정상적인 RenderingGameData 입니다.");
			return nullptr;
		}

		return _characterGameData->_renderingGameData;
	}

	float CommonCharacterActorComponent::getCurrentStat_(const CharacterStatType statType) const noexcept
	{
		return _currentStats[static_cast<uint8>(statType)];
	}

	bool CommonCharacterActorComponent::setCurrentStat_(const CharacterStatType statType, const float value) noexcept
	{
		if (nullptr == _characterGameData)
		{
			return false;
		}

		if (0.0f > value)
		{
			_currentStats[static_cast<uint8>(statType)] = 0.0f;
		}
		else
		{
			float targetValue = value;

			if (CharacterStatType::Hp == statType
				|| CharacterStatType::Mp == statType)
			{
				const float maxStatValue = _characterGameData->getStat(static_cast<CharacterStatType>(static_cast<uint8>(statType) + 1));
				if (maxStatValue < value)
				{
					targetValue = maxStatValue;
				}
			}

			_currentStats[static_cast<uint8>(statType)] = targetValue;
		}

		return true;
	}

	bool CommonCharacterActorComponent::addCurrentStat_(const CharacterStatType statType, const float value) noexcept
	{
		const float currentStat = getCurrentStat_(statType);
		const float targetValue = value + currentStat;
		return setCurrentStat_(statType, targetValue);
	}

	float CommonCharacterActorComponent::getCurrentHp_(void) const noexcept
	{
		return getCurrentStat_(CharacterStatType::Hp);
	}

	bool CommonCharacterActorComponent::setCurrentHp_(const float value) noexcept
	{
		return setCurrentStat_(CharacterStatType::Hp, value);
	}

	BuffObject* CommonCharacterActorComponent::getBuff_(const BuffGameDataKey& buffGameDataKey) noexcept
	{
		auto target = _currentBuffSet.find(buffGameDataKey.getKeyValue());
		if (_currentBuffSet.end() == target)
		{
			return nullptr;
		}

		return target->second;
	}

	bool CommonCharacterActorComponent::registerBuff_(const BuffGameDataKey& buffGameDataKey, BuffObject* buffObject) noexcept
	{
		if (false == buffGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (nullptr == buffObject)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (nullptr != getBuff_(buffGameDataKey))
		{
			TA_LOG_DEV("사용중인 버프입니다.");
			return false;
		}

		auto rv = _currentBuffSet.insert(std::pair<BuffGameDataKey, BuffObject*>(buffGameDataKey.getKeyValue(), buffObject));
		if (false == rv.second)
		{
			return false;
		}

		return true;
	}

	bool CommonCharacterActorComponent::unregisterBuff_(const BuffGameDataKey& buffGameDataKey) noexcept
	{
		if (false == buffGameDataKey.isValid())
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		if (nullptr == getBuff_(buffGameDataKey))
		{
			TA_LOG_DEV("사용중인 버프가 아닙니다.");
			return false;
		}

		if (1 != _currentBuffSet.erase(buffGameDataKey))
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		return true;
	}

	void CommonCharacterActorComponent::onChangedCharacterGameData(void) noexcept
	{
		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "character 게임데이터가 없습니다.");
			return;
		}

		const uint8 count = static_cast<uint8>(CharacterStatType::Count);
		for (uint8 index = 0; index < count; ++index)
		{
			_currentStats[index] = _characterGameData->getStat(static_cast<CharacterStatType>(index));
		}
	}
}

