//---------------------------------------------------------------------------------
// Vec2.h
//---------------------------------------------------------------------------------
//
// Basic Implementation of a 2d Vector
//
#pragma once

#include <string>

class Vec2
{
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
    Vec2 operator/(float rhs) const;

    const Vec2& operator*=(float rhs);
    const Vec2& operator/=(float rhs);

    float operator[](int idx) const;
    float& operator[](int idx);

    const Vec2& Normalize();
    Vec2 Cross(float rhs) const;
    float Cross(const Vec2& rhs) const;
    float GetMagnitude() const;
    float GetMagnitudeSquared() const;

    bool IsValid() const;

    std::string ToString() const;

    float Dot(const Vec2& rhs) const
    {
        return X * rhs.X + Y * rhs.Y;
    }

    const float* ToPtr() const
    {
        return &X;
    }


    static float GetAngleBetween(const Vec2& a, const Vec2& b)
    {
        float dot = a.Dot(b);
        float magA = a.GetMagnitude();
        float magB = b.GetMagnitude();

        // Calculate the cosine of the angle
        float cosTheta = dot / (magA * magB);

        // Calculate the angle in radians
        float angle = acos(cosTheta);

        if (a.Cross(b) < 0)
        {
            angle = 2 * 3.141 - angle; 
        }

        return angle; // Return the angle in radians
    }


    float X;
    float Y;
};
