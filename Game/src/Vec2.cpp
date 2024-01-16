#include "stdafx.h"
#include "Vec2.h"
#include <math.h>
#include <cassert>

Vec2::Vec2() : X(0), Y(0)
{
}

Vec2::Vec2(const float value) : X(value), Y(value)
{
}

Vec2::Vec2(const Vec2& rhs) : X(rhs.X), Y(rhs.Y)
{
}

Vec2::Vec2(float X, float Y) : X(X), Y(Y)
{
}

Vec2::Vec2(const float* xy) : X(xy[0]), Y(xy[1])
{
}

Vec2& Vec2::operator=(const Vec2& rhs)
{
    X = rhs.X;
    Y = rhs.Y;
    return *this;
}

bool Vec2::operator==(const Vec2& rhs) const
{
    if (std::abs(X - rhs.X) < std::numeric_limits<float>::epsilon()) return false;
    if (std::abs(Y - rhs.Y) < std::numeric_limits<float>::epsilon()) return false;
    return true;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    if (*this == rhs) return false;
    return true;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    Vec2 temp;
    temp.X = X + rhs.X;
    temp.Y = Y + rhs.Y;
    return temp;
}

const Vec2& Vec2::operator+=(const Vec2& rhs)
{
    X += rhs.X;
    Y += rhs.Y;
    return *this;
}

const Vec2& Vec2::operator-=(const Vec2& rhs)
{
    X -= rhs.X;
    Y -= rhs.Y;
    return *this;
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    Vec2 temp;
    temp.X = X - rhs.X;
    temp.Y = Y - rhs.Y;
    return temp;
}

Vec2 Vec2::operator*(const float rhs) const
{
    Vec2 temp;
    temp.X = X * rhs;
    temp.Y = Y * rhs;
    return temp;
}

Vec2 Vec2::operator/(float rhs) const
{
    return Vec2(X / rhs, Y / rhs);
}

const Vec2& Vec2::operator*=(const float rhs)
{
    X *= rhs;
    Y *= rhs;
    return *this;
}

const Vec2& Vec2::operator/=(const float rhs)
{
    X /= rhs;
    Y /= rhs;
    return *this;
}

float Vec2::operator[](const int idx) const
{
    assert(idx >= 0 && idx < 2);
    return (&X)[idx];
}

float& Vec2::operator[](const int idx)
{
    assert(idx >= 0 && idx < 2);
    return (&X)[idx];
}

const Vec2& Vec2::Normalize()
{
    float mag = GetMagnitude();
    float invMag = 1.0f / mag;
    if (0.0f * invMag == 0.0f * invMag)
    {
        X = X * invMag;
        Y = Y * invMag;
    }
    return *this;
}

float Vec2::Cross(const Vec2& rhs) const
{
    return X * rhs.Y - Y * rhs.X;
}


Vec2 Vec2::Cross(float rhs) const
{
    return {Y * rhs, X * -rhs};
}

float Vec2::GetMagnitude() const
{
    float mag = X * X + Y * Y;
    mag = sqrtf(mag);
    return mag;
}

float Vec2::GetMagnitudeSquared() const
{
    return X * X + Y * Y;
}

bool Vec2::IsValid() const
{
    if (X * 0.0f != X * 0.0f)
    {
        // x is NaN or Inf
        return false;
    }
    if (Y * 0.0f != Y * 0.0f)
    {
        // y is NaN or Inf
        return false;
    }
    return true;
}

std::string Vec2::ToString() const
{
    return "{" + std::to_string(X) + "," + std::to_string(Y) + "}";
}
