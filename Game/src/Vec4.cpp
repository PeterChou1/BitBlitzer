#include "stdafx.h"
#include "Vec4.h"
#include <math.h>
#include <cassert>


Vec4::Vec4() : X(0), Y(0), Z(0), W(0)
{
}

Vec4::Vec4(const Vec3& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(1)
{
}

Vec4::Vec4(const float value) : X(value), Y(value), Z(value), W(value)
{
}

Vec4::Vec4(const Vec4& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W)
{
}

Vec4::Vec4(float X, float Y, float Z, float W) : X(X), Y(Y), Z(Z), W(W)
{
}

Vec4::Vec4(const float* rhs)
{
    X = rhs[0];
    Y = rhs[1];
    Z = rhs[2];
    W = rhs[3];
}

Vec4& Vec4::operator=(const Vec4& rhs)
{
    X = rhs.X;
    Y = rhs.Y;
    Z = rhs.Z;
    W = rhs.W;
    return *this;
}

bool Vec4::operator==(const Vec4& rhs) const
{
    if (X != rhs.X) return false;
    if (Y != rhs.Y) return false;
    if (Z != rhs.Z) return false;
    if (W != rhs.W) return false;
    return true;
}

bool Vec4::operator!=(const Vec4& rhs) const
{
    if (*this == rhs) return false;
    return true;
}

Vec4 Vec4::operator+(const Vec4& rhs) const
{
    Vec4 temp;
    temp.X = X + rhs.X;
    temp.Y = Y + rhs.Y;
    temp.Z = Z + rhs.Z;
    temp.W = W + rhs.W;
    return temp;
}

const Vec4& Vec4::operator+=(const Vec4& rhs)
{
    X += rhs.X;
    Y += rhs.Y;
    Z += rhs.Z;
    W += rhs.W;
    return *this;
}

const Vec4& Vec4::operator-=(const Vec4& rhs)
{
    X -= rhs.X;
    Y -= rhs.Y;
    Z -= rhs.Z;
    W -= rhs.W;
    return *this;
}

const Vec4& Vec4::operator*=(const Vec4& rhs)
{
    X *= rhs.X;
    Y *= rhs.Y;
    Z *= rhs.Z;
    W *= rhs.W;
    return *this;
}

const Vec4& Vec4::operator/=(const Vec4& rhs)
{
    X /= rhs.X;
    Y /= rhs.Y;
    Z /= rhs.Z;
    W /= rhs.W;
    return *this;
}

Vec4 Vec4::operator-(const Vec4& rhs) const
{
    Vec4 temp;
    temp.X = X - rhs.X;
    temp.Y = Y - rhs.Y;
    temp.Z = Z - rhs.Z;
    temp.W = W - rhs.W;
    return temp;
}

Vec4 Vec4::operator*(float rhs) const
{
    Vec4 temp;
    temp.X = X * rhs;
    temp.Y = Y * rhs;
    temp.Z = Z * rhs;
    temp.W = W * rhs;
    return temp;
}

float Vec4::operator[](int idx) const
{
    assert(idx >= 0 && idx < 4);
    return (&X)[idx];
}

float& Vec4::operator[](const int idx)
{
    assert(idx >= 0 && idx < 4);
    return (&X)[idx];
}

float Vec4::Dot(const Vec4& rhs) const
{
    float xx = X * rhs.X;
    float yy = Y * rhs.Y;
    float zz = Z * rhs.Z;
    float ww = W * rhs.W;
    return (xx + yy + zz + ww);
}

const Vec4& Vec4::Normalize()
{
    float mag = GetMagnitude();
    float invMag = 1.0f / mag;

    if (0.0f * invMag == 0.0f * invMag)
    {
        X *= invMag;
        Y *= invMag;
        Z *= invMag;
        W *= invMag;
    }

    return *this;
}

float Vec4::GetMagnitude() const
{
    float mag;
    mag = X * X + Y * Y + Z * Z + W * W;
    mag = sqrtf(mag);
    return mag;
}

bool Vec4::IsValid() const
{
    if (X * 0.0f != X * 0.0f)
    {
        return false;
    }
    if (Y * 0.0f != Y * 0.0f)
    {
        return false;
    }
    if (Z * 0.0f != Z * 0.0f)
    {
        return false;
    }
    if (W * 0.0f != W * 0.0f)
    {
        return false;
    }
    return true;
}

Vec3 Vec4::ToVec3() const
{
    if (W != 0)
    {
        return Vec3(X / W, Y / W, Z / W);
    }
    return Vec3(X, Y, Z);
}

std::string Vec4::toString() const
{
    return "{" + std::to_string(X) + "," + std::to_string(Y) + "," + std::to_string(Z) + "," + std::to_string(W) + "}";
}
