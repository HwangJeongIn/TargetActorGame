#include "TAConvertFunctions.h"


void TAVectorToFVector(const ta::Vector input, FVector& output) noexcept
{
	output.X = input._x;
	output.Y = input._y;
	output.Z = input._z;
}

extern void FVectorToTAVector(const FVector input, ta::Vector& output) noexcept
{
	output._x = input.X;
	output._y = input.Y;
	output._z = input.Z;
}