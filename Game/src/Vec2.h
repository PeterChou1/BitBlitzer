#pragma once
#include <string>

/*
================================
Vec2
================================
*/
class Vec2 {
public:
    Vec2();
    Vec2(float value);
    Vec2(const Vec2& rhs);
    Vec2(float X, float Y);
    Vec2(const float* xy);

    Vec2& operator=(const Vec2& rhs);

    bool operator==(const Vec2& rhs) const;
    bool operator!=(const Vec2& rhs) const;

    Vec2 operator+(const Vec2& rhs) const;
    const Vec2& operator+=(const Vec2& rhs);


    Vec2 operator-(const Vec2& rhs) const;
    const Vec2& operator-=(const Vec2& rhs);

    Vec2 operator*(float rhs) const;

    const Vec2& operator*=(float rhs);
    const Vec2& operator/=(float rhs);

    float operator[](int idx) const;
    float& operator[](int idx);

    const Vec2& Normalize();
    float GetMagnitude() const;
    bool IsValid() const;

    std::string ToString() const;

    float Dot(const Vec2& rhs) const 
    {
        return x * rhs.x + y * rhs.y;
    }
    const float* ToPtr() const 
    {
        return &x;
    }

public:
    float x;
    float y;
};
