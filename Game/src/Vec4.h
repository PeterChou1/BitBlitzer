#pragma once
#include "Vec3.h"
class Vec4 {
public:
    Vec4();
    Vec4(const Vec3& rhs);
    Vec4(const float value);
    Vec4(const Vec4& rhs);
    Vec4(float X, float Y, float Z, float W);
    Vec4(const float* rhs);

    Vec4& operator=(const Vec4& rhs);
    bool operator==(const Vec4& rhs) const;
    bool operator!=(const Vec4& rhs) const;
    Vec4 operator+(const Vec4& rhs) const;
    const Vec4& operator+=(const Vec4& rhs);
    const Vec4& operator-=(const Vec4& rhs);
    const Vec4& operator*=(const Vec4& rhs);
    const Vec4& operator/=(const Vec4& rhs);
    Vec4 operator-(const Vec4& rhs) const;
    Vec4 operator*(float rhs) const;
    float operator[](int idx) const;
    float& operator[](int idx);

    float Dot(const Vec4& rhs) const;
    const Vec4& Normalize();
    float GetMagnitude() const;
    bool IsValid() const;
    Vec3 ToVec3() const;

    void Zero() { x = 0; y = 0; z = 0; w = 0; }
    const float* ToPtr() const { return &x; }
    float* ToPtr() { return &x; }

    std::string toString() const;
    

public:
    float x, y, z, w;
};