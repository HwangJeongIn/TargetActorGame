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
		_characterGameDataKey.clear();
		_aiGameDataKey.clear();
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

			_moveGameDataKey = FromStringCast<MoveGameDataKeyType>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("CharacterGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "CharacterGameDataKey 로드 실패");
				return false;
			}

			_characterGameDataKey = FromStringCast<CharacterGameDataKeyType>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("AiGameDataKey");
			if (nullptr != value) // 플레이어인 경우 없을 수 있다.
			{
				_aiGameDataKey = FromStringCast<AiGameDataKeyType>(*value);
			}
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
		_actorType = ActorType::Count;
		_interactionTypes.clear();
		_strength = 0.0f;
		_agility = 0.0f;
		_maxHp = 0.0f;
		_openDialog.clear();
		_renderingGameDataKey.clear();
	}

	bool CharacterGameData::loadXml(XmlNode* xmlNode) noexcept
	{

		{
			const std::string* value = xmlNode->getAttribute("Name");
			if (nullptr != value) // 없을 수 있다.
			{
				_name = *value;
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("ActorType");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "ActorType 로드 실패");
				return false;
			}

			_actorType = ConvertStringToEnum<ActorType>(*value);
			if (ActorType::Count == _actorType)
			{
				TA_ASSERT_DEV(false, "ActorType 로드 실패 %s", ToTstring(*value).c_str());
				return false;
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("InteractionType");
			if (nullptr != value) // 없을 수 있다.
			{
				std::vector<std::string> splitedStrings;
				Split(*value, ",", splitedStrings);
				const uint32 count = splitedStrings.size();

				// 최대 3가지 인터랙션을 넣을 수 있다.
				// ex) Greet,Talk,Gather
				if (count > 5)
				{
					TA_ASSERT_DEV(false, "InteractionType 로드 실패, 최대 3개까지 가능합니다. %s", ToTstring(*value).c_str());
					return false;
				}

				InteractionType interactionType;
				for (uint32 index = 0; index < count; index += 2)
				{
					interactionType = ConvertStringToEnum<InteractionType>(splitedStrings[index]);
					if (InteractionType::Count == interactionType)
					{
						TA_ASSERT_DEV(false, "InteractionType 로드 실패, 비정상적인 타입 %s", ToTstring(splitedStrings[index]).c_str());
						return false;
					}
					_interactionTypes.push_back(interactionType);
				}
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

		{
			const std::string* value = xmlNode->getAttribute("OpenDialog");
			if (nullptr != value) // 없을 수 있다.
			{
				_openDialog = *value;
			}
		}

		{
			const std::string* value = xmlNode->getAttribute("RenderingGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "RenderingGameDataKey 로드 실패");
				return false;
			}

			_renderingGameDataKey = FromStringCast<RenderingGameDataKeyType>(*value);
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
		_renderingGameDataKey.clear();
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

		
		{
			const std::string* value = xmlNode->getAttribute("RenderingGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "RenderingGameDataKey 로드 실패");
				return false;
			}

			_renderingGameDataKey = FromStringCast<RenderingGameDataKeyType>(*value);
		}
		

		return true;
	}
	
	const ItemType ItemGameData::getItemType(void) const noexcept
	{
		return _itemType;
	}
}


namespace ta
{
	RenderingGameData::RenderingGameData(void) noexcept
	{
	}

	RenderingGameData::~RenderingGameData(void) noexcept
	{
	}

	GameDataType RenderingGameData::getGameDataType(void) noexcept
	{
		return GameDataType::RenderingGameData;
	}

	bool RenderingGameData::loadXml(XmlNode* xmlNode) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("MeshType");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MeshType 로드 실패");
				return false;
			}

			_meshType = ConvertStringToEnum<MeshType>(*value);
			if (MeshType::Count == _meshType)
			{
				TA_ASSERT_DEV(false, "MeshType 로드 실패 %s", ToTstring(*value).c_str());
				return false;
			}
		}


		{
			const std::string* value = xmlNode->getAttribute("MeshPath");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MeshPath 로드 실패");
				return false;
			}

			std::vector<std::string> splitedStrings;
			Split(*value, "'", splitedStrings);
			const uint32 count = splitedStrings.size();
			if (1 == count)
			{
				_meshPath = splitedStrings[0];
			}
			else if (4 == count)
			{
				_meshPath = splitedStrings[2];
			}
			else
			{
				TA_ASSERT_DEV(false, "MeshPath 로드 실패");
				return false;
			}
		}
		
		if(MeshType::Skeletal == _meshType)
		{
			const std::string* value = xmlNode->getAttribute("AnimInstancePath");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "AnimInstancePath 로드 실패");
				return false;
			}

			std::vector<std::string> splitedStrings;
			Split(*value, "'", splitedStrings);
			const uint32 count = splitedStrings.size();
			if (1 == count)
			{
				_animInstancePath = splitedStrings[0];
			}
			else if (4 == count)
			{
				_animInstancePath = splitedStrings[2];
			}
			else
			{
				TA_ASSERT_DEV(false, "AnimInstancePath 로드 실패");
				return false;
			}
		}

		return true;
	}

	void RenderingGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_meshType = MeshType::Count;
		_meshPath.clear();
		_animInstancePath.clear();
	}
}