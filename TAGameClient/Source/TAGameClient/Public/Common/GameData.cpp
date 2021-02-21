#include "Common/GameData.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"


namespace ta
{
	GameData::GameData(void) noexcept
		: _isValid(false)
	{
	}

	GameData::~GameData(void) noexcept
	{
	}

	bool GameData::isValid(void) const noexcept
	{
		return _isValid;
	}

	void GameData::clear(void) noexcept
	{
		_isValid = false;
		clearDetail();
	}

	void GameData::setIsValid(bool input) noexcept
	{
		_isValid = input;
	}
}


namespace ta
{
	GroupGameData::GroupGameData(void) noexcept
	{
	}

	GroupGameData::~GroupGameData(void) noexcept
	{
	}

	GameDataType GroupGameData::getGameDataType(void) noexcept
	{
		return GameDataType::GroupGameData;
	}
	
	void GroupGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_moveGameDataKey.clear();
		_aiGameDataKey.clear();
		_characterGameDataKey.clear();
	}

	bool GroupGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("MoveGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MoveGameDataKey 로드 실패");
				return false;
			}

			_moveGameDataKey = Atoi(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("AiGameDataKey");
			if (nullptr != value) // 플레이어인 경우 없을 수 있다.
			{
				_aiGameDataKey = Atoi(*value);
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("CharacterGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "CharacterGameDataKey 로드 실패");
				return false;
			}

			_characterGameDataKey = Atoi(*value);
		}

		return true;
	}
}


namespace ta
{
	MoveGameData::MoveGameData(void) noexcept
	{
	}

	MoveGameData::~MoveGameData(void) noexcept
	{
	}

	GameDataType MoveGameData::getGameDataType(void) noexcept
	{
		return GameDataType::MoveGameData;
	}
	
	void MoveGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_speed = 0.0f;
	}
	bool MoveGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("Speed");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "Speed 로드 실패");
				return false;
			}

			_speed = Atof(*value);
		}

		return true;
	}
}


namespace ta
{
	AiGameData::AiGameData(void) noexcept
	{
	}

	AiGameData::~AiGameData(void) noexcept
	{
	}

	GameDataType AiGameData::getGameDataType(void) noexcept
	{
		return GameDataType::AiGameData;
	}
	
	void AiGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_attackRange = 0.0f;
	}

	bool AiGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("AttackRange");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "AttackRange 로드 실패");
				return false;
			}

			_attackRange = Atof(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("AiClassType");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "AiClassType 로드 실패");
				return false;
			}
			_aiClassType = ConvertStringToEnum<AiClassType>(*value);
			TA_ASSERT_DEV((AiClassType::Count != _aiClassType), "AiClassType 비정상입니다.");
		}

		return true;
	}
}


namespace ta
{
	CharacterGameData::CharacterGameData(void) noexcept
	{
	}

	CharacterGameData::~CharacterGameData(void) noexcept
	{
	}

	GameDataType CharacterGameData::getGameDataType(void) noexcept
	{
		return GameDataType::CharacterGameData;
	}
	
	void CharacterGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_strength = 0.0f;
		_agility = 0.0f;
		_maxHp = 0.0f;
	}

	bool CharacterGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("ActorType");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "ActorType 로드 실패");
				return false;
			}

			_actorType = ConvertStringToEnum<ActorType>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("Strength");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "Strength 로드 실패");
				return false;
			}

			_strength = Atof(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("Agility");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "Agility 로드 실패");
				return false;
			}

			_agility = Atof(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("MaxHp");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MaxHp 로드 실패");
				return false;
			}

			_maxHp = Atof(*value);
		}

		return true;
	}
}


namespace ta
{
	ItemGameData::ItemGameData(void) noexcept
	{
	}

	ItemGameData::~ItemGameData(void) noexcept
	{
	}

	GameDataType ItemGameData::getGameDataType(void) noexcept
	{
		return GameDataType::ItemGameData;
	}
	
	void ItemGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_itemType = ItemType::Count;
	}

	bool ItemGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		//{
		//	const std::string* value = xmlNode->getAttribute("ActorType");
		//	if (nullptr == value)
		//	{
		//		TA_ASSERT_DEV(false, "ActorType 로드 실패");
		//		return false;
		//	}
		//
		//	_actorType = ConvertStringToEnum<ActorType>(*value);
		//}

		return true;
	}
	
	const ItemType ItemGameData::getItemType(void) const noexcept
	{
		return _itemType;
	}
}