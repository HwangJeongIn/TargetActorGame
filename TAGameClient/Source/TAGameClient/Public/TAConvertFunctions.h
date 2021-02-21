#pragma once

#include "TAGameClient.h"
#include "Common/Vector.h"


extern void TAVectorToFVector(const ta::Vector input, FVector& output) noexcept;
extern void FVectorToTAVector(const FVector input, ta::Vector& output) noexcept;