#pragma once

namespace ta
{
	class Vector
	{
	public:
		float _x;
		float _y;
		float _z;

	public:
		static const Vector Zero;

	public:
		Vector(const float x =0.0f, const float y = 0.0f, const float z = 0.0f) noexcept;
		~Vector(void) noexcept;

		Vector operator+(const Vector& v) const noexcept;
		Vector operator-(const Vector& v) const noexcept;
		bool operator==(const Vector& v) const noexcept;
		bool operator!=(const Vector& v) const noexcept;
		Vector operator/(const Vector& v) const noexcept;

		Vector& operator+=(const Vector& v) noexcept;
		Vector& operator-=(const Vector& v) noexcept;

		bool normalize(void) noexcept;
		float size(void) const noexcept;
		float sizeSquared(void) const noexcept;
	};
}