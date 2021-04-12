#pragma once

#include "Common/CommonBase.h"
#include "Common/ActorComponentTypeList.h"
#include <unordered_map>


namespace ta
{
	class ActorComponent;
}


namespace ta
{
	class ActorComponentPool
	{
	public:
		ActorComponentPool(void) noexcept;
		virtual ~ActorComponentPool(void) noexcept;

		bool addComponentCountFromActorType(const ActorType& ownerActorType, const uint32& count) noexcept;

		template <typename T>
		bool allocatePoolFromInitializedCount(void) noexcept
		{
			if (0 == _count)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

			_isValid = true;
			_actorComponents = new T(_count);
			return true;
		}

		template <typename T>
		T* getActorComponent(const ActorType& targetActorType, const uint32& relativeGroupIndex) noexcept
		{
			std::unordered_map<ActorType, ActorComponentGroupData>::const_iterator actorComponentGroupData = ActorComponentGroups.find(targetActorType);
			if (ActorComponentGroups.end() == actorComponentGroupData)
			{
				TA_ASSERT_DEV(false, "비정상");
				return false;
			}

			if (actorComponentGroupData->second._countPerComponent <= relativeGroupIndex)
			{
				TA_ASSERT_DEV(false, "비정상적인 인덱스 입니다. ActorType : %d, Index : %d", static_cast<uint8>(targetActorType), relativeGroupIndex);
				return false;
			}

			std::unordered_map<ActorType, uint32>::const_iterator it = _startIndexMap.find(targetActorType);
			if (_startIndexMap.end() == it)
			{																																		
				TA_ASSERT_DEV(false, "비정상");																										
				return false;																														
			}

			return static_cast<T*>(_actorComponents[it->second + relativeGroupIndex]);
		}

	private:
		void clear(void) noexcept;

	private:
		bool _isValid;
		ActorComponent* _actorComponents;
		uint32 _count;
		std::unordered_map<ActorType, uint32> _startIndexMap;
	};
}