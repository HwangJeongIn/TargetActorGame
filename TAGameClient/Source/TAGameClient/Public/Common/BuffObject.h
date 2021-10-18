#pragma once
#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"
#include "Common/Lockable.h"


namespace ta
{
	class BuffGameData;
	class ContentParameter;
}


namespace ta
{
	class BuffObject : public Lockable
	{
	public:
		BuffObject(void) noexcept;
		virtual ~BuffObject(void) noexcept;

		bool initializeBuffObject(const BuffGameData* buffGameData) noexcept;

		BuffState doBuff(const ContentParameter& parameter) noexcept;
		bool undoBuff(const ContentParameter& parameter) const noexcept;

		uint32 getInterval(void) const noexcept;
		bool isOneTime(void) const noexcept;

	private:
		const BuffGameData* _buffGameData;
		uint32 _remainingTime;
	};
}