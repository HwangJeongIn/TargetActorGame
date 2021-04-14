#pragma once

#include "Common/ActorComponent.h"
#include "Common/GameData.h"


namespace ta
{
	class ItemGameData;
	class RenderingGameData;
}


namespace ta
{
	class CommonObjectActorComponent : public ActorComponent
	{
	public:
		virtual ~CommonObjectActorComponent(void) noexcept;
		virtual bool initializeData(const ComponentData* componentData) noexcept override;
		virtual ComponentData* packData_(void) noexcept override;

		static const ActorComponentType getActorComponentType(void) noexcept;

		const RenderingGameData* getRenderingGameData_(void) const noexcept;

	protected:
		CommonObjectActorComponent(void) noexcept;

	private:
		const ItemGameData* _itemGameData;
		const RenderingGameData* _renderingGameData;
	};
}
