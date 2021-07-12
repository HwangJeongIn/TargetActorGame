#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include <vector>
#include <string>


namespace ta
{
	class XmlNode;
	class ContentParameter;
	class ConditionGameDataObject;
	class GameDataManager;
	class MoveGameData;
	class GroupGameData;
	class CharacterGameData;
	class AiGameData;
	class RenderingGameData;
}


namespace ta
{
	class GameDataLoadHelper
	{
	public:
		explicit GameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~GameDataLoadHelper(void) noexcept;

	public:
		const GameDataManager* _gameDataManager; // 현재는 아직 사용 안한다.
	};


	class GameData
	{
	public:
		GameData(void) noexcept;
		virtual ~GameData(void) noexcept;

		bool isValid(void) const noexcept;
		void clear(void) noexcept;

		//virtual bool loadXml(XmlNode* xmlNode, GameDataLoadHelper* loadHelper) noexcept = 0;
		//virtual bool finishLoading(const GameDataLoadHelper* loadHelper) noexcept = 0;
		//virtual bool finalCheck(const GameDataManager* gameDataManager) noexcept = 0;

		void setIsValid(bool input) noexcept;

		// private 스코프 안에 있어도 상속받은 클래스에서 재정의 가능하다. => 다른것들도 다 바꿔야할듯..
		// 효과 : 외부에서 사용 불가능하다. 내부에서 사용할 경우에는 무조건 이렇게 쓰는게 좋을듯하다.
		//        상속받은 클래스에서 부모클래스의 가상함수를 호출할 수 없다. 호출해야할 특별한 케이스라면 protected 사용
	private:
		virtual void clearDetail(void) noexcept = 0;

	protected:
		bool _isValid;
	};
} 


namespace ta
{
	class GroupGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit GroupGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~GroupGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		GroupGameDataKey _key;

		MoveGameDataKey _moveGameDataKey;
		CharacterGameDataKey _characterGameDataKey;
		AiGameDataKey _aiGameDataKey;
	};


	class GroupGameData : public GameData
	{
	public:
		GroupGameData(void) noexcept;
		virtual ~GroupGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, GroupGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const GroupGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		GroupGameDataKey _key;

		const MoveGameData* _moveGameData;
		const CharacterGameData* _characterGameData;
		const AiGameData* _aiGameData;
	};
} 


namespace ta
{
	class MoveGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit MoveGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~MoveGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		MoveGameDataKey _key;
	};


	class MoveGameData : public GameData
	{
	public:
		MoveGameData(void) noexcept;
		virtual ~MoveGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, MoveGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const MoveGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		MoveGameDataKey _key;

		float _speed;
	};
} 


namespace ta
{
	class AiGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit AiGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~AiGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		AiGameDataKey _key;
	};


	class AiGameData : public GameData
	{
	public:
		AiGameData(void) noexcept;
		virtual ~AiGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, AiGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const AiGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		AiGameDataKey _key;

		AiClassType _aiClassType;
		PathPointPathKey _pathPointPathKey;
		float _attackRange;
	};
}


namespace ta
{
	class CharacterGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit CharacterGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~CharacterGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		CharacterGameDataKey _key;
		RenderingGameDataKey _renderingGameDataKey;
	};


	class CharacterGameData : public GameData
	{
	public:
		CharacterGameData(void) noexcept;
		virtual ~CharacterGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, CharacterGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const CharacterGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		CharacterGameDataKey _key;

		std::string _name;
		ActorType _actorType;
		//AiGameDataKey _aiGameDataKey;
		std::vector<InteractionType> _interactionTypes;

		float _strength;
		float _agility;
		float _maxHp;

		std::string _openDialog;

		const RenderingGameData* _renderingGameData;
	};
}


namespace ta
{
	class ItemGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit ItemGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~ItemGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		ItemGameDataKey _key;
		RenderingGameDataKey _renderingGameDataKey;
	};


	class ItemGameData : public GameData
	{
	public:
		ItemGameData(void) noexcept;
		virtual ~ItemGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, ItemGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const ItemGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

		const ItemType getItemType(void) const noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		ItemGameDataKey _key;
		ItemType _itemType;
		std::string _name;

		const RenderingGameData* _renderingGameData;
		std::string _iconPath;

	};
}


namespace ta
{
	class RenderingGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit RenderingGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~RenderingGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		RenderingGameDataKey _key;
	};


	class RenderingGameData : public GameData
	{
	public:
		RenderingGameData(void) noexcept;
		virtual ~RenderingGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, RenderingGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const RenderingGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		RenderingGameDataKey _key;

		MeshType _meshType;
		std::string _meshPath;
		std::string _animInstancePath;

	};
}


namespace ta
{
	class ConditionGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit ConditionGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~ConditionGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		ConditionGameDataKey _key;
	};


	class ConditionGameData : public GameData
	{
	public:
		ConditionGameData(void) noexcept;
		virtual ~ConditionGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, ConditionGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const ConditionGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

		bool checkCondition(ContentParameter& parameter) const noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		ConditionGameDataKey _key;
		const ConditionGameDataObject* _conditionObject;
	};
}


namespace ta
{
	class EventGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit EventGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~EventGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		EventGameDataKey _key;
	};


	class EventGameData : public GameData
	{
	public:
		EventGameData(void) noexcept;
		virtual ~EventGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, EventGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const EventGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		EventGameDataKey _key;
		uint32 _interval;
		std::vector<const ConditionGameData*> _conditionGameData;
	};
}

namespace ta
{
	class SectorZoneGameDataLoadHelper : public GameDataLoadHelper
	{
	public:
		explicit SectorZoneGameDataLoadHelper(const GameDataManager* gameDataManager) noexcept;
		virtual ~SectorZoneGameDataLoadHelper(void) noexcept;

		void clear(void) noexcept;

	public:
		SectorZoneGameDataKey _key;
	};


	class SectorZoneGameData : public GameData
	{
	public:
		SectorZoneGameData(void) noexcept;
		virtual ~SectorZoneGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		bool loadXml(XmlNode* xmlNode, SectorZoneGameDataLoadHelper* loadHelper) noexcept;
		bool finishLoading(const SectorZoneGameDataLoadHelper* loadHelper) noexcept;
		bool finalCheck(const GameDataManager* gameDataManager) noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		SectorZoneGameDataKey _key;

	};
}
