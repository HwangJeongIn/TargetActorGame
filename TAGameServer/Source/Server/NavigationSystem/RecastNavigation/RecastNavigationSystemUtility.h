#pragma once

#include "Common/Vector.h"

extern ta::Vector TAVectorToRecastVector(const ta::Vector input) noexcept;
extern ta::Vector RecastVectorToTAVector(const ta::Vector input) noexcept;
extern ta::Vector RecastVectorToTAVector(const float* input) noexcept;