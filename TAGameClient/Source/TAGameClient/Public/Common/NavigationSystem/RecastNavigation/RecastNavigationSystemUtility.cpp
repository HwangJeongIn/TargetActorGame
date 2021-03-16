#include "RecastNavigationSystemUtility.h"

ta::Vector TAVectorToRecastVector(const ta::Vector input) noexcept
{
    return ta::Vector(-input._x, input._z, -input._y);
}

ta::Vector RecastVectorToTAVector(const ta::Vector input) noexcept
{
    return ta::Vector(-input._x, -input._z, input._y);
}

ta::Vector RecastVectorToTAVector(const float* input) noexcept
{
    return ta::Vector(-input[0], -input[2], input[1]);
}
