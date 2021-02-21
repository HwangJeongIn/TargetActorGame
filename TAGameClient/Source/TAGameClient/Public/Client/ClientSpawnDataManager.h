#pragma once
#include "Common/CommonBase.h"
#include "Common/CommonSpawnDataManager.h"


namespace ta
{
	class ClientSpawnDataManager : public CommonSpawnDataManager
	{
	public:
		ClientSpawnDataManager(void) noexcept;
		virtual ~ClientSpawnDataManager(void) noexcept;

		virtual bool initialize(void) noexcept override final;
		virtual bool open(void) noexcept override final;
		virtual void close(void) noexcept override final;

	};
}
