#pragma once

#include "Common/CommonBase.h"
#include "Common/Vector.h"
#include <vector>


namespace ta
{
	class PathPoint
	{
	public:
		explicit PathPoint(const Vector& position = Vector::Zero) noexcept;
		virtual ~PathPoint(void) noexcept;
		
		const Vector& getPosition(void) const noexcept;

	private:
		Vector _position;
	};
}
