//---------------------------------------------------------------------------------
// Quat.h
//---------------------------------------------------------------------------------
//
// Basic Implementation of a Quaternion
//
#pragma once
#include "Vec3.h"
#include "Mat3.h"
#include "Vec4.h"

class Quat
{
public:
    Quat();
    Quat(const Quat& rhs);
    Quat(float X, float Y, float Z, float W);
    Quat(Vec3 n, float angleRadians);
    const Quat& operator=(const Quat& rhs);
    Quat& operator*=(const float& rhs);
    Quat& operator*=(const Quat& rhs);
    Quat operator*(const Quat& rhs) const;
    void Normalize();
    void Invert();
    Quat Inverse() const;
    float MagnitudeSquared() const;
    float GetMagnitude() const;
    Vec3 RotatePoint(const Vec3& rhs) const;
    Mat3 RotateMatrix(const Mat3& rhs) const;
    void GetEulerAngles(float& roll, float& pitch, float& yaw) const;
    Vec3 xyz() const { return Vec3(X, Y, Z); }
    Mat3 ToMat3() const;
    Vec4 ToVec4() const { return Vec4(W, X, Y, Z); }
    static Quat FromRotationMatrix(Mat3& m);
    static Quat Lerp(const Quat& quat1, const Quat& quat2, float t);
    static Quat Slerp(const Quat& quat1, const Quat& quat2, float t);
    float W;
    float X;
    float Y;
    float Z;
};
