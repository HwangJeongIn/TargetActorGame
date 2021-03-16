#pragma once

#include "Common/Vector.h"

namespace ta
{
	extern Vector TAVectorToRecastVector(const ta::Vector input) noexcept;
	extern Vector RecastVectorToTAVector(const ta::Vector input) noexcept;
	extern Vector RecastVectorToTAVector(const float* input) noexcept;
}