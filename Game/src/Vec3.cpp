#include "stdafx.h"
#include "Vec3.h"
#include "Mesh.h"
#include <math.h>
#include <cassert>

Vec3::Vec3() :
    x(0),
    y(0),
    z(0) {
}

Vec3::Vec3(float value) :
    x(value),
    y(value),
    z(value) {
}

Vec3::Vec3(const Vec3& rhs) :
    x(rhs.x),
    y(rhs.y),
    z(rhs.z) {
}

Vec3::Vec3(float X, float Y, float Z) :
    x(X),
    y(Y),
    z(Z) {
}

Vec3::Vec3(const float* xyz) :
    x(xyz[0]),
    y(xyz[1]),
    z(xyz[2]) {
}

Vec3& Vec3::operator = (const Vec3& rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
}

Vec3& Vec3::operator = (const float* rhs) {
    x = rhs[0];
    y = rhs[1];
    z = rhs[2];
    return *this;
}

bool Vec3::operator == (const Vec3& rhs) const {
    if (x != rhs.x) {
        return false;
    }
    if (y != rhs.y) {
        return false;
    }
    if (z != rhs.z) {
        return false;
    }
    return true;
}

bool Vec3::operator != (const Vec3& rhs) const {
    if (*this == rhs) {
        return false;
    }
    return true;
}

Vec3 Vec3::operator + (const Vec3& rhs) const {
    Vec3 temp;
    temp.x = x + rhs.x;
    temp.y = y + rhs.y;
    temp.z = z + rhs.z;
    return temp;
}

const Vec3& Vec3::operator += (const Vec3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

const Vec3& Vec3::operator -= (const Vec3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vec3 Vec3::operator - (const Vec3& rhs) const {
    Vec3 temp;
    temp.x = x - rhs.x;
    temp.y = y - rhs.y;
    temp.z = z - rhs.z;
    return temp;
}

Vec3 Vec3::operator * (const float rhs) const {
    Vec3 temp;
    temp.x = x * rhs;
    temp.y = y * rhs;
    temp.z = z * rhs;
    return temp;
}

Vec3 Vec3::operator*(const Transform& rhs) const
{
    Vec3 t = *this + rhs.position;
    return rhs.rotation.RotatePoint(t);
}

Vec3 Vec3::operator / (const float rhs) const {
    Vec3 temp;
    temp.x = x / rhs;
    temp.y = y / rhs;
    temp.z = z / rhs;
    return temp;
}

const Vec3& Vec3::operator*=(const float rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

const Vec3& Vec3::operator*=(const Transform& rhs)
{
    *this = *this + rhs.position;
    *this = rhs.rotation.RotatePoint(*this);
    return *this;
}

const Vec3& Vec3::operator/=(const float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

float Vec3::operator[](const int idx) const {
    assert(idx >= 0 && idx < 3);
    return (&x)[idx];
}

float& Vec3::operator[](const int idx) {
    assert(idx >= 0 && idx < 3);
    return (&x)[idx];
}

Vec3 Vec3::Cross(const Vec3& rhs) const {
    Vec3 temp;
    temp.x = (y * rhs.z) - (rhs.y * z);
    temp.y = (rhs.x * z) - (x * rhs.z);
    temp.z = (x * rhs.y) - (rhs.x * y);
    return temp;
}

float Vec3::Dot(const Vec3& rhs) const {
    float temp = (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
    return temp;
}

const Vec3& Vec3::Normalize() {
    float mag = GetMagnitude();
    float invMag = 1.0f / mag;
    if (0.0f * invMag == 0.0f * invMag) {
        x *= invMag;
        y *= invMag;
        z *= invMag;
    }
    return *this;
}

float Vec3::GetMagnitude() const {
    float mag;
    mag = x * x + y * y + z * z;
    mag = sqrtf(mag);
    return mag;
}

bool Vec3::IsValid() const {
    if (x * 0.0f != x * 0.0f) {
        return false;
    }
    if (y * 0.0f != y * 0.0f) {
        return false;
    }
    if (z * 0.0f != z * 0.0f) {
        return false;
    }
    return true;
}

void Vec3::GetOrtho(Vec3& u, Vec3& v) const {
    Vec3 n = *this;
    n.Normalize();
    const Vec3 w = (n.z * n.z > 0.9f * 0.9f) ? Vec3(1, 0, 0) : Vec3(0, 0, 1);
    u = w.Cross(n);
    u.Normalize();
    v = n.Cross(u);
    v.Normalize();
    u = v.Cross(n);
    u.Normalize();
}

std::string Vec3::toString() const
{
    return "{" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "}";
}
