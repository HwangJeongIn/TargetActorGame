#pragma once

#include "Common/CommonMoveActorComponent.h"
#include "Common/KeyDefinition.h"


namespace ta
{
	class ServerMoveActorComponent : public CommonMoveActorComponent
	{
	public:
		ServerMoveActorComponent(void) noexcept;
		virtual ~ServerMoveActorComponent(void) noexcept;

		virtual void onActive(void) noexcept override final;

		const std::unordered_set<ActorKey>& getViewList_(void) const noexcept;
		void setViewList_(const std::unordered_set<ActorKey>& newViewList) noexcept;

		bool addToViewList_(const ActorKey& target) noexcept;
		bool removeFromViewList_(const ActorKey& target) noexcept;

		bool checkExistInViewList_(const ActorKey& target) const noexcept;

	private:
		std::unordered_set<ActorKey> _viewList;
	};
}
