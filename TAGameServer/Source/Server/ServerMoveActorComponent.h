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

		bool addToViewList_(const ActorKey& target, const ActorType& targetActorType) noexcept;
		bool removeFromViewList_(const ActorKey& target, const ActorType& targetActorType) noexcept;

		bool checkExistInViewList_(const ActorKey& target) const noexcept;

		bool existPlayerInViewList_(void) const noexcept;
		uint32 getViewListPlayerCount_(void) const noexcept;

	private:
		void increaseViewListPlayerCount_(void) noexcept;
		void decreaseViewListPlayerCount_(void) noexcept;


		// dtQuerySpecialLinkFilterget

	private:
		std::unordered_set<ActorKey> _viewList;
		//dtQuerySpecialLinkFilter _defaultQuerySpecialLinkFilter;
		uint32 _viewListPlayerCount; // 현재 플레이어가 뷰리스트에 얼마나 있는지 // 캐싱
	};
}
