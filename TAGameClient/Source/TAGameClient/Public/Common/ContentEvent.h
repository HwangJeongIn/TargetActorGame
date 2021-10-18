#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Socket.h"
#include <chrono>


namespace ta
{
	class ContentEventObject
	{
	public:
		ContentEventObject(void) noexcept;
		~ContentEventObject(void) noexcept;

		void clear(void) noexcept;
		virtual void clearDetail(void) noexcept;

		constexpr bool operator<(const ContentEventObject& _left) const noexcept;
		void setWakeupTime(const long long delayMilliSec) noexcept;

	public:
		
		// 공통 정보	
		ContentEventType _contentEventType;

		ActorKey _myActorKey;
		ActorKey _targetActorKey;

		// 자동으로 채워진다.
	public:
		std::chrono::high_resolution_clock::time_point _wakeupTime;
		float _deltaTime;
	};
}


namespace ta
{
	class ContentEventAiObject : public ContentEventObject
	{
	public:
		ContentEventAiObject(void) noexcept;
		~ContentEventAiObject(void) noexcept;

		virtual void clearDetail(void) noexcept override final;

	public:
		AiCommandType _aiCommandType;
	};
}


namespace ta
{
	class ContentEventBuffObject : public ContentEventObject
	{
	public:
		ContentEventBuffObject(void) noexcept;
		~ContentEventBuffObject(void) noexcept;

		virtual void clearDetail(void) noexcept override final;

	public:
		BuffGameDataKey _buffGameDataKey;
	};
}


namespace ta
{
	class ContentEventSectorObject : public ContentEventObject
	{
	public:
		ContentEventSectorObject(void) noexcept;
		~ContentEventSectorObject(void) noexcept;

		virtual void clearDetail(void) noexcept override final;

	public:

		SectorKey _sectorKey;
		bool _sectorEventIndex;
		bool _isBasicSectorEvent;
	};
}


namespace ta
{
	class ContentEventOverlappedStruct : public OverlappedStructBase
	{
	public:
		ContentEventOverlappedStruct(void) noexcept;
		virtual ~ContentEventOverlappedStruct(void) noexcept;

		void clear(void) noexcept;

	public:
		ContentEventObject* _contentEventObject;
	};
}