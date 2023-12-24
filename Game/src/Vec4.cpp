#include "stdafx.h"
#include "Vec4.h"
#include <math.h>
#include <cassert>


Vec4::Vec4() : x(0), y(0), z(0), w(0) {}

Vec4::Vec4(const Vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(1) {}

Vec4::Vec4(const float value) : x(value), y(value), z(value), w(value) {}

Vec4::Vec4(const Vec4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

Vec4::Vec4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

Vec4::Vec4(const float* rhs) {
    x = rhs[0];
    y = rhs[1];
    z = rhs[2];
    w = rhs[3];
}

Vec4& Vec4::operator=(const Vec4& rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

bool Vec4::operator==(const Vec4& rhs) const {
    if (x != rhs.x) return false;
    if (y != rhs.y) return false;
    if (z != rhs.z) return false;
    if (w != rhs.w) return false;
    return true;
}

bool Vec4::operator!=(const Vec4& rhs) const {
    if (*this == rhs) return false;
    return true;
}

Vec4 Vec4::operator+(const Vec4& rhs) const {
    Vec4 temp;
    temp.x = x + rhs.x;
    temp.y = y + rhs.y;
    temp.z = z + rhs.z;
    temp.w = w + rhs.w;
    return temp;
}

const Vec4& Vec4::operator+=(const Vec4& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

const Vec4& Vec4::operator-=(const Vec4& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

const Vec4& Vec4::operator*=(const Vec4& rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

const Vec4& Vec4::operator/=(const Vec4& rhs) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

Vec4 Vec4::operator-(const Vec4& rhs) const {
    Vec4 temp;
    temp.x = x - rhs.x;
    temp.y = y - rhs.y;
    temp.z = z - rhs.z;
    temp.w = w - rhs.w;
    return temp;
}

Vec4 Vec4::operator*(float rhs) const {
    Vec4 temp;
    temp.x = x * rhs;
    temp.y = y * rhs;
    temp.z = z * rhs;
    temp.w = w * rhs;
    return temp;
}

float Vec4::operator[](int idx) const {
    assert(idx >= 0 && idx < 4);
    return (&x)[idx];
}

float& Vec4::operator[](const int idx) {
    assert(idx >= 0 && idx < 4);
    return (&x)[idx];
}

float Vec4::Dot(const Vec4& rhs) const {
    float xx = x * rhs.x;
    float yy = y * rhs.y;
    float zz = z * rhs.z;
    float ww = w * rhs.w;
    return (xx + yy + zz + ww);
}

const Vec4& Vec4::Normalize() {
    float mag = GetMagnitude();
    float invMag = 1.0f / mag;

    if (0.0f * invMag == 0.0f * invMag) {
        x *= invMag;
        y *= invMag;
        z *= invMag;
        w *= invMag;
    }

    return *this;
}

float Vec4::GetMagnitude() const {
    float mag;
    mag = x * x + y * y + z * z + w * w;
    mag = sqrtf(mag);
    return mag;
}

bool Vec4::IsValid() const {
    if (x * 0.0f != x * 0.0f) {
        return false;
    }
    if (y * 0.0f != y * 0.0f) {
        return false;
    }
    if (z * 0.0f != z * 0.0f) {
        return false;
    }
    if (w * 0.0f != w * 0.0f) {
        return false;
    }
    return true;
}

Vec3 Vec4::ToVec3() const
{
    if (w != 0) {
        return Vec3(x / w, y / w, z / w);
    }
    return Vec3(x, y, z);
}

std::string Vec4::toString() const
{
    return "{" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(w) + "}";
}
