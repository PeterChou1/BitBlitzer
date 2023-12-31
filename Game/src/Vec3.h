#pragma once
#include <string>

struct Transform;

/*
===================
Vec3
===================
*/
class Vec3 {
public:
    Vec3();
    Vec3(float value);
    Vec3(const Vec3& rhs);
    Vec3(float X, float Y, float Z);
    Vec3(const float* xyz);

    Vec3& operator=(const Vec3& rhs);
    Vec3& operator=(const float* rhs);

    bool operator==(const Vec3& rhs) const;
    bool operator!=(const Vec3& rhs) const;

    Vec3 operator+(const Vec3& rhs) const;
    const Vec3& operator+=(const Vec3& rhs);
    const Vec3& operator-=(const Vec3& rhs);

    Vec3 operator-(const Vec3& rhs) const;
    Vec3 operator*(const float rhs) const;
    Vec3 operator/(const float rhs) const;

    const Vec3& operator*=(const float rhs);
    const Vec3& operator/=(const float rhs);

    float operator[](const int idx) const;
    float& operator[](const int idx);

    void Zero() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vec3 Cross(const Vec3& rhs) const;
    float Dot(const Vec3& rhs) const;
    Vec3& Normalize();
    float GetMagnitude() const;
    float GetLengthSqr() const {
        return Dot(*this);
    }

    bool IsValid() const;

    void GetOrtho(Vec3& u, Vec3& v) const;

    const float* ToPtr() const {
        return &x;
    }

    std::string ToString() const;

public:
    float x;
    float y;
    float z;
};


// return a vector from line start to a point on the plane
Vec3 IntersectPlane(const Vec3& point, const Vec3& normal, Vec3& start, Vec3& end, float& scale);

float Dist(const Vec3& point, const Vec3& planeN, const Vec3& planeP);
