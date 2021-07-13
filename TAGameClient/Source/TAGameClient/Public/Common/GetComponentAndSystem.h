#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/CommonApp.h"
#include "Common/CommonActor.h"
#include "Common/CommonActorManager.h"
#include "Common/CommonActorSystemManager.h"
#include "Common/ActorComponent.h"



namespace ta
{
	template <typename T>
	T* GetActorComponent(CommonActor* actor) noexcept
	{
		return static_cast<T*>(GetActorComponent(actor, T::getActorComponentType()));
	}

	template <typename T>
	T* GetActorComponent(const ActorKey& actorKey) noexcept
	{
		return static_cast<T*>(GetActorComponent(actorKey, T::getActorComponentType()));
	}

	// 타입으로 찾을 때 전체 타입에 대해서 쭉 찾아볼 수 있음
	extern ActorComponent* GetActorComponent(CommonActor* actor, ActorComponentType type) noexcept;
	extern ActorComponent* GetActorComponent(const ActorKey& actorKey, ActorComponentType type) noexcept;
}


namespace ta
{
	template <typename T>
	T* GetActorSystem(void) noexcept
	{
		return static_cast<T*>(GetActorSystem(T::getActorSystemType()));
	}

	extern ActorSystem* GetActorSystem(ActorSystemType type) noexcept;
}


namespace ta
{
	template <typename T, typename TKey>
	const T* GetGameData(const TKey& key) noexcept
	{
		return static_cast<const T*>(g_app->getGameDataManager()->getGameData(key.getKeyValue(), T::getGameDataType()));
	}
}
