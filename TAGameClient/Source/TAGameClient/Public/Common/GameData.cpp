#include "Common/GameData.h"
#include "Common/StringUtility.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/ConditionGameDataObject.h"
#include "Common/GameDataManager.h"
#include "Common/GetComponentAndSystem.h"


namespace ta
{
	GameDataLoadHelper::GameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: _gameDataManager(gameDataManager)
	{
	}

	GameDataLoadHelper::~GameDataLoadHelper(void) noexcept
	{
	}


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
		clearDetail();
		_isValid = false;
	}

	void GameData::setIsValid(bool input) noexcept
	{
		_isValid = input;
	}
}


namespace ta
{
	GroupGameDataLoadHelper::GroupGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}

	GroupGameDataLoadHelper::~GroupGameDataLoadHelper(void) noexcept
	{
	}

	void GroupGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
		_moveGameDataKey.clear();
		_characterGameDataKey.clear();
		_aiGameDataKey.clear();
	}


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

	bool GroupGameData::loadFromXml(XmlNode* xmlNode, GroupGameDataLoadHelper* loadHelper) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("MoveGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "MoveGameDataKey 로드 실패");
				return false;
			}

			loadHelper->_moveGameDataKey = FromStringCast<MoveGameDataKeyType>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("CharacterGameDataKey");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "CharacterGameDataKey 로드 실패");
				return false;
			}

			loadHelper->_characterGameDataKey = FromStringCast<CharacterGameDataKeyType>(*value);
		}

		{
			const std::string* value = xmlNode->getAttribute("AiGameDataKey");
			if (nullptr != value) // 플레이어인 경우 없을 수 있다.
			{
				loadHelper->_aiGameDataKey = FromStringCast<AiGameDataKeyType>(*value);
			}
		}

		return true;
	}

	bool GroupGameData::finishLoading(const GroupGameDataLoadHelper* loadHelper) noexcept
	{
		_moveGameData = GetGameData<MoveGameData>(loadHelper->_moveGameDataKey);
		if (nullptr == _moveGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_characterGameData = GetGameData<CharacterGameData>(loadHelper->_characterGameDataKey);
		if (nullptr == _characterGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_aiGameData = GetGameData<AiGameData>(loadHelper->_aiGameDataKey);
		if (nullptr == _aiGameData)
		{
			// 없을 수 있다.
			//TA_ASSERT_DEV(false, "비정상");
			//return false;
		}

		return true;
	}

	bool GroupGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}
	
	void GroupGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_moveGameData = nullptr;
		_characterGameData = nullptr;
		_aiGameData = nullptr;
	}
}


namespace ta
{
	MoveGameDataLoadHelper::MoveGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}
	
	MoveGameDataLoadHelper::~MoveGameDataLoadHelper(void) noexcept
	{
	}

	void MoveGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


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

	bool MoveGameData::loadFromXml(XmlNode* xmlNode, MoveGameDataLoadHelper* loadHelper) noexcept
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

	bool MoveGameData::finishLoading(const MoveGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool MoveGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}
	
	void MoveGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_speed = 0.0f;
	}
}


namespace ta
{
	AiGameDataLoadHelper::AiGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}

	AiGameDataLoadHelper::~AiGameDataLoadHelper(void) noexcept
	{
	}

	void AiGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


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

	bool AiGameData::loadFromXml(XmlNode* xmlNode, AiGameDataLoadHelper* loadHelper) noexcept
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

	bool AiGameData::finishLoading(const AiGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool AiGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}
	
	void AiGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_aiClassType = AiClassType::Count;
		_pathPointPathKey.clear();
		_attackRange = 0.0f;
	}
}


namespace ta
{
	CharacterGameDataLoadHelper::CharacterGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}

	CharacterGameDataLoadHelper::~CharacterGameDataLoadHelper(void) noexcept
	{
	}

	void CharacterGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
		_renderingGameDataKey.clear();
	}


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

	bool CharacterGameData::loadFromXml(XmlNode* xmlNode, CharacterGameDataLoadHelper* loadHelper) noexcept
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

			loadHelper->_renderingGameDataKey = FromStringCast<RenderingGameDataKeyType>(*value);
		}

		return true;
	}

	bool CharacterGameData::finishLoading(const CharacterGameDataLoadHelper* loadHelper) noexcept
	{
		_renderingGameData = GetGameData<RenderingGameData>(loadHelper->_renderingGameDataKey);
		if (nullptr == _renderingGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		return true;
	}

	bool CharacterGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return false;
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
		_renderingGameData = nullptr;
	}
}


namespace ta
{
	ItemGameDataLoadHelper::ItemGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}

	ItemGameDataLoadHelper::~ItemGameDataLoadHelper(void) noexcept
	{
	}

	void ItemGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
		_renderingGameDataKey.clear();
	}


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

	bool ItemGameData::loadFromXml(XmlNode* xmlNode, ItemGameDataLoadHelper* loadHelper) noexcept
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

			loadHelper->_renderingGameDataKey = FromStringCast<RenderingGameDataKeyType>(*value);
		}


		return true;
	}

	bool ItemGameData::finishLoading(const ItemGameDataLoadHelper* loadHelper) noexcept
	{
		_renderingGameData = GetGameData<RenderingGameData>(loadHelper->_renderingGameDataKey);
		if (nullptr == _renderingGameData)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		return true;
	}

	bool ItemGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}
	
	void ItemGameData::clearDetail(void) noexcept
	{
		_key.clear();
		_itemType = ItemType::Count;
		_renderingGameData = nullptr;
	}
	
	const ItemType ItemGameData::getItemType(void) const noexcept
	{
		return _itemType;
	}
}


namespace ta
{
	RenderingGameDataLoadHelper::RenderingGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}

	RenderingGameDataLoadHelper::~RenderingGameDataLoadHelper(void) noexcept
	{
	}

	void RenderingGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


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

	bool RenderingGameData::loadFromXml(XmlNode* xmlNode, RenderingGameDataLoadHelper* loadHelper) noexcept
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

		if (MeshType::Skeletal == _meshType)
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

	bool RenderingGameData::finishLoading(const RenderingGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool RenderingGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
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

namespace ta
{
	ConditionGameDataLoadHelper::ConditionGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}
	
	ConditionGameDataLoadHelper::~ConditionGameDataLoadHelper(void) noexcept
	{
	}

	void ConditionGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


	ConditionGameData::ConditionGameData(void) noexcept
	{
	}

	ConditionGameData::~ConditionGameData(void) noexcept
	{
	}

	GameDataType ConditionGameData::getGameDataType(void) noexcept
	{
		return GameDataType::ConditionGameData;
	}

	bool ConditionGameData::loadFromXml(XmlNode* xmlNode, ConditionGameDataLoadHelper* loadHelper) noexcept
	{
		{
			const std::string* value = xmlNode->getAttribute("ConditionString");
			if (nullptr == value)
			{
				TA_ASSERT_DEV(false, "ConditionString 로드 실패");
				return false;
			}

			std::vector<std::string> splitedStrings;
			Split(*value, "()!,", splitedStrings);
			const uint32 count = splitedStrings.size();
			if (3 > count)
			{
				TA_ASSERT_DEV(false, "ConditionString 로드 실패");
				return false;
			}

			std::string conditionGameDataObjectString;
			bool isNot = false;
			uint32 offset = 0;
			if (0 == splitedStrings[0].compare("!"))
			{
				if (4 > count)
				{
					TA_ASSERT_DEV(false, "ConditionString 로드 실패");
					return false;
				}

				conditionGameDataObjectString = splitedStrings[1];
				isNot = true;
				offset = 3;
			}
			else
			{
				conditionGameDataObjectString = splitedStrings[0];
				isNot = false;
				offset = 2;
			}

			std::vector <std::string> dataStrings;
			const uint32 splitedStringsCount = splitedStrings.size();
			for (uint32 index = offset; index < (splitedStringsCount - 1); ++index)
			{
				dataStrings.push_back(splitedStrings[index]);
			}

			const ConditionGameDataObjectType objectType = ConvertStringToEnum<ConditionGameDataObjectType>(conditionGameDataObjectString);
			ConditionGameDataObjectFactory factory;
			_conditionObject = factory.generateConditionGameDataObject(dataStrings, objectType, isNot);

			if (nullptr == _conditionObject)
			{
				TA_ASSERT_DEV(false, "ConditionString 로드 실패");
				return false;
			}
		}

		return true;
	}


	bool ConditionGameData::finishLoading(const ConditionGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool ConditionGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}

	bool ConditionGameData::checkCondition(ContentParameter& parameter) const noexcept
	{
		return _conditionObject->checkCondition(parameter);
	}

	void ConditionGameData::clearDetail(void) noexcept
	{
		_key.clear();
		if (true == _isValid)
		{
			if (nullptr != _conditionObject)
			{
				delete _conditionObject;
				_conditionObject = nullptr;
			}
		}
		else
		{
			_conditionObject = nullptr;
		}
	}
}


namespace ta
{
	EventGameDataLoadHelper::EventGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}
	
	EventGameDataLoadHelper::~EventGameDataLoadHelper(void) noexcept
	{
	}

	void EventGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


	EventGameData::EventGameData(void) noexcept
	{
	}

	EventGameData::~EventGameData(void) noexcept
	{
	}

	GameDataType EventGameData::getGameDataType(void) noexcept
	{
		return GameDataType::EventGameData;
	}

	bool EventGameData::loadFromXml(XmlNode* xmlNode, EventGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool EventGameData::finishLoading(const EventGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool EventGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}

	void EventGameData::clearDetail(void) noexcept
	{
	}
}


namespace ta
{
	SectorZoneGameDataLoadHelper::SectorZoneGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept
		: GameDataLoadHelper(gameDataManager)
	{
	}
	
	SectorZoneGameDataLoadHelper::~SectorZoneGameDataLoadHelper(void) noexcept
	{
	}

	void SectorZoneGameDataLoadHelper::clear(void) noexcept
	{
		_key.clear();
	}


	SectorZoneGameData::SectorZoneGameData(void) noexcept
	{
	}

	SectorZoneGameData::~SectorZoneGameData(void) noexcept
	{
	}

	GameDataType SectorZoneGameData::getGameDataType(void) noexcept
	{
		return GameDataType::SectorZoneGameData;
	}

	bool SectorZoneGameData::loadFromXml(XmlNode* xmlNode, SectorZoneGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool SectorZoneGameData::finishLoading(const SectorZoneGameDataLoadHelper* loadHelper) noexcept
	{
		return true;
	}

	bool SectorZoneGameData::checkFinally(const GameDataManager* gameDataManager) noexcept
	{
		return true;
	}

	void SectorZoneGameData::clearDetail(void) noexcept
	{
	}
}