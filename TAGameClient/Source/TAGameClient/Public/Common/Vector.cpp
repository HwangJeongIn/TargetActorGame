﻿#include "Common/Vector.h"
#include <cmath>

namespace ta
{
	const Vector Vector::Zero(0.0f, 0.0f, 0.0f);

	Vector::Vector(const float x, const float y, const float z) noexcept
		: _x(x), _y(y), _z(z)
	{
	}

	Vector::~Vector(void) noexcept
	{
	}

	Vector Vector::operator+(const Vector& v) const noexcept
	{
		return Vector(_x + v._x, _y + v._y, _z + v._z);
	}

	Vector Vector::operator-(const Vector& v) const noexcept
	{
		return Vector(_x - v._x, _y - v._y, _z - v._z);
	}

	bool Vector::operator==(const Vector& v) const noexcept
	{
		return (_x == v._x && _y == v._y && _z == v._z);
	}

	bool Vector::operator!=(const Vector& v) const noexcept
	{
		return (_x != v._x || _y != v._y || _z != v._z);
	}

	Vector Vector::operator/(const Vector& v) const noexcept
	{
		return Vector(_x / v._x, _y / v._y, _z / v._z);
	}

	Vector Vector::operator/(float scale) const noexcept
	{
		const float rScale = 1.f / scale;
		return Vector(_x * rScale, _y * rScale, _z * rScale);
	}

	Vector Vector::operator*(const Vector& v) const noexcept
	{
		return Vector(_x * v._x, _y * v._y, _z * v._z);
	}

	Vector Vector::operator*(float scale) const noexcept
	{
		return Vector(_x * scale, _y * scale, _z * scale);
	}

	Vector& Vector::operator+=(const Vector& v) noexcept
	{
		_x += v._x;
		_y += v._y;
		_z += v._z;

		return *this;
	}

	Vector& Vector::operator-=(const Vector& v) noexcept
	{
		_x -= v._x;
		_y -= v._y;
		_z -= v._z;

		return *this;
	}

	bool Vector::normalize(void) noexcept
	{
		const float vectorSize = size();
		if (0 == vectorSize)
		{
			return false;
		}

		const float invSize = 1 / vectorSize;

		_x *= invSize;
		_y *= invSize;
		_z *= invSize;

		return true;
	}

	float Vector::size(void) const noexcept
	{
		return sqrtf(_x * _x + _y * _y + _z * _z);
	}

	float Vector::sizeSquared(void) const noexcept
	{
		return (_x * _x + _y * _y + _z * _z);
	}
}
