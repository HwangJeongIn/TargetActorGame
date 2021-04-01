#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include <vector>
#include <string>


namespace ta
{
	class XmlNode;
}


namespace ta
{
	class GameData
	{
	public:
		GameData(void) noexcept;
		virtual ~GameData(void) noexcept;

		bool isValid(void) const noexcept;
		void clear(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept =0;
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
	class GroupGameData : public GameData
	{
	public:
		GroupGameData(void) noexcept;
		virtual ~GroupGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept override final;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		GroupGameDataKey _key;

		MoveGameDataKey _moveGameDataKey;
		CharacterGameDataKey _characterGameDataKey;
		AiGameDataKey _aiGameDataKey;
	};
} 


namespace ta
{
	class MoveGameData : public GameData
	{
	public:
		MoveGameData(void) noexcept;
		virtual ~MoveGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept override final;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		MoveGameDataKey _key;

		float _speed;
	};
} 


namespace ta
{
	class AiGameData : public GameData
	{
	public:
		AiGameData(void) noexcept;
		virtual ~AiGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept override final;

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
	class CharacterGameData : public GameData
	{
	public:
		CharacterGameData(void) noexcept;
		virtual ~CharacterGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept override final;

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

		std::string _skeletalMeshPath;
		std::string _animInstancePath;
	};
}


namespace ta
{
	class ItemGameData : public GameData
	{
	public:
		ItemGameData(void) noexcept;
		virtual ~ItemGameData(void) noexcept;

		static GameDataType getGameDataType(void) noexcept;

		virtual bool loadXml(XmlNode* xmlNode) noexcept override final;

		const ItemType getItemType(void) const noexcept;

	private:
		virtual void clearDetail(void) noexcept override final;

	public:
		ItemGameDataKey _key;
		ItemType _itemType;
		std::string _name;
		std::string _iconPath;
		std::string _meshPath;

	};
}
