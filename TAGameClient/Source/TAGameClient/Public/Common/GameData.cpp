﻿#include "Common/GameData.h"
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

			_moveGameDataKey = FromStringCast<int32>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("CharacterGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "CharacterGameDataKey 로드 실패");
				return false;
			}

			_characterGameDataKey = FromStringCast<int32>(*value);
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

			_speed = FromStringCast<float>(*value);
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
		_aiClassType = AiClassType::Count;
		_pathPointPathKey.clear();
		_attackRange = 0.0f;
	}

	bool AiGameData::loadXml(XmlNode* xmlNode) noexcept
	{
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

		{
			const std::string* value = xmlNode->getAttribute("PathPointPathKey");
			if (nullptr != value) // 경로는 없을 수 있다.
			{
				hash_value stringHash = ComputeHash(*value);
				_pathPointPathKey = stringHash;
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("AttackRange");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "AttackRange 로드 실패");
				return false;
			}

			_attackRange = FromStringCast<float>(*value);
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
		_aiGameDataKey.clear();
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
			const std::string* value = xmlNode->getAttribute("AiGameDataKey");
			if (nullptr != value) // 플레이어인 경우 없을 수 있다.
			{
				_aiGameDataKey = FromStringCast<int32>(*value);
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("Strength");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "Strength 로드 실패");
				return false;
			}

			_strength = FromStringCast<float>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("Agility");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "Agility 로드 실패");
				return false;
			}

			_agility = FromStringCast<float>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("MaxHp");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MaxHp 로드 실패");
				return false;
			}

			_maxHp = FromStringCast<float>(*value);
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
		// itemType
		{
			const std::string* value = xmlNode->getAttribute("ItemType");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "ItemType 로드 실패");
				return false;
			}
		
			_itemType = ConvertStringToEnum<ItemType>(*value);
		}

		// name
		{
			const std::string* value = xmlNode->getAttribute("Name");
			if (nullptr == value)
			{
				TA_LOG_DEV("Name 존재 X");
			}
			else
			{
				_name = *value;
			}
		}

		// iconPath
		{
			const std::string* value = xmlNode->getAttribute("IconPath");
			if (nullptr == value)
			{
				TA_LOG_DEV("IconPath 존재 X");
			}
			else
			{
				_iconPath = *value;
			}
		}

		// meshPath
		{
			const std::string* value = xmlNode->getAttribute("MeshPath");
			if (nullptr == value)
			{
				TA_LOG_DEV("MeshPath 존재 X");
			}
			else
			{
				_meshPath = *value;
			}
		}

		return true;
	}
	
	const ItemType ItemGameData::getItemType(void) const noexcept
	{
		return _itemType;
	}
}