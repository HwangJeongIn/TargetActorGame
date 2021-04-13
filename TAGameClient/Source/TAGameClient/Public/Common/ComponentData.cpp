#include "Common/ComponentData.h"

namespace ta
{
	ComponentData::ComponentData(const ActorComponentType& type) noexcept
		: _type(type)
	{
	}

	ComponentData::~ComponentData(void) noexcept
	{
	}

	const ActorComponentType ComponentData::getActorComponentType(void) const noexcept
	{
		return _type;
	}
}


namespace ta
{
	CommonMoveComponentData::CommonMoveComponentData(void) noexcept
		: ComponentData(ActorComponentType::Move)
	{
		clear();
	}

	CommonMoveComponentData::~CommonMoveComponentData(void) noexcept
	{
	}

	void CommonMoveComponentData::clear(void) noexcept
	{
		_position = Vector(0.0f, 0.0f, 0.0f);
		_rotation = Vector(0.0f, 0.0f, 0.0f);
		_speed = 0.0f;
	}
}


namespace ta
{
	CommonActionComponentData::CommonActionComponentData(void) noexcept
		: ComponentData(ActorComponentType::Action)
	{
		clear();
	}
	
	CommonActionComponentData::~CommonActionComponentData(void) noexcept
	{
	}

	void CommonActionComponentData::clear(void) noexcept
	{
	}
}


namespace ta
{
	CommonAiComponentData::CommonAiComponentData(void) noexcept
		: ComponentData(ActorComponentType::Ai)
	{
		clear();
	}

	CommonAiComponentData::~CommonAiComponentData(void) noexcept
	{
	}

	void CommonAiComponentData::clear(void) noexcept
	{
		_aiClassType = AiClassType::Count;
		_pathPointPathKey.clear();
	}
}


namespace ta
{
	CommonCharacterComponentData::CommonCharacterComponentData(void) noexcept
		: ComponentData(ActorComponentType::Character)
	{
		clear();
	}

	CommonCharacterComponentData::~CommonCharacterComponentData(void) noexcept
	{
	}

	void CommonCharacterComponentData::clear(void) noexcept
	{
		_characterGameDataKey.clear();
		_currentHp =0;
	}
}


namespace ta
{
	ItemElementData::ItemElementData(void) noexcept
	{
		clear();
	}

	ItemElementData::~ItemElementData(void) noexcept
	{
	}

	void ItemElementData::clear(void) noexcept
	{
		_baseKey.clear();
		_detail.clear_();
		_stackCount = 0;
	}
}


namespace ta
{
	CommonInventoryComponentData::CommonInventoryComponentData(void) noexcept
		: ComponentData(ActorComponentType::Inventory)
	{
		clear();
	}

	CommonInventoryComponentData::~CommonInventoryComponentData(void) noexcept
	{
	}

	void CommonInventoryComponentData::clear(void) noexcept
	{
		_itemSetType = ItemSetType::Count;
		_capacity = 0;
		//_itemElementDataSet.clear();
	}
}


namespace ta
{
	CommonObjectComponentData::CommonObjectComponentData(void) noexcept
		: ComponentData(ActorComponentType::Object)
	{
		clear();
	}

	CommonObjectComponentData::~CommonObjectComponentData(void) noexcept
	{
	}

	void CommonObjectComponentData::clear(void) noexcept
	{
		_itemGameDataKey.clear();
		_renderingGameDataKey.clear();
	}
}