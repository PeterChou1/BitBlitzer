#include "stdafx.h"
#include "Vec2.h"
#include <math.h>
#include <cassert>

Vec2::Vec2() : x(0), y(0) {}

Vec2::Vec2(const float value) : x(value), y(value) {}

Vec2::Vec2(const Vec2& rhs) : x(rhs.x), y(rhs.y) {}

Vec2::Vec2(float X, float Y) : x(X), y(Y) {}

Vec2::Vec2(const float* xy) : x(xy[0]), y(xy[1]) {}

Vec2& Vec2::operator=(const Vec2& rhs) {
    x = rhs.x;
    y = rhs.y;
    return *this;
}

bool Vec2::operator==(const Vec2& rhs) const {
    if (x != rhs.x) return false;
    if (y != rhs.y) return false;
    return true;
}

bool Vec2::operator!=(const Vec2& rhs) const {
    if (*this == rhs) return false;
    return true;
}

Vec2 Vec2::operator+(const Vec2& rhs) const {
    Vec2 temp;
    temp.x = x + rhs.x;
    temp.y = y + rhs.y;
    return temp;
}

const Vec2& Vec2::operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

const Vec2& Vec2::operator-=(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vec2 Vec2::operator-(const Vec2& rhs) const {
    Vec2 temp;
    temp.x = x - rhs.x;
    temp.y = y - rhs.y;
    return temp;
}

Vec2 Vec2::operator*(const float rhs) const {
    Vec2 temp;
    temp.x = x * rhs;
    temp.y = y * rhs;
    return temp;
}

const Vec2& Vec2::operator*=(const float rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

const Vec2& Vec2::operator/=(const float rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
}

float Vec2::operator[](const int idx) const {
    assert(idx >= 0 && idx < 2);
    return (&x)[idx];
}

float& Vec2::operator[](const int idx) {
    assert(idx >= 0 && idx < 2);
    return (&x)[idx];
}

const Vec2& Vec2::Normalize() {
    float mag = GetMagnitude();
    float invMag = 1.0f / mag;
    if (0.0f * invMag == 0.0f * invMag) {
        x = x * invMag;
        y = y * invMag;
    }
    return *this;
}

float Vec2::GetMagnitude() const {
    float mag;
    mag = x * x + y * y;
    mag = sqrtf(mag);
    return mag;
}

bool Vec2::IsValid() const {
    if (x * 0.0f != x * 0.0f) { // x is NaN or Inf
        return false;
    }
    if (y * 0.0f != y * 0.0f) { // y is NaN or Inf
        return false;
    }
    return true;
}

std::string Vec2::toString() const
{
    return "{" + std::to_string(x) + "," + std::to_string(y) + "}";
}




