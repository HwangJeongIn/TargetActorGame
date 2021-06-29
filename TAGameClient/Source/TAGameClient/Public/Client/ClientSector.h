#pragma once

#include "Common/Sector.h"


namespace ta
{
	class ClientSector : public Sector
	{
	public:
		ClientSector(void) noexcept;
		virtual ~ClientSector(void) noexcept override final;
	};
}