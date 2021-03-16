#include "RecastNavigation/RecastNavigationSystemUtility.h"

namespace ta
{
    Vector TAVectorToRecastVector(const ta::Vector input) noexcept
    {
        return ta::Vector(-input._x, input._z, -input._y);
    }

    Vector RecastVectorToTAVector(const ta::Vector input) noexcept
    {
        return ta::Vector(-input._x, -input._z, input._y);
    }

    Vector RecastVectorToTAVector(const float* input) noexcept
    {
        return ta::Vector(-input[0], -input[2], input[1]);
    }
}