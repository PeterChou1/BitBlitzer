#include "stdafx.h"
#include "Quat.h"
#include <math.h>
#include <cmath>;

Quat::Quat() : X(0), Y(0), Z(0), W(1)
{
}

Quat::Quat(const Quat& rhs) : X(rhs.X), Y(rhs.Y), Z(rhs.Z), W(rhs.W)
{
}

Quat::Quat(float X, float Y, float Z, float W) : X(X), Y(Y), Z(Z), W(W)
{
}

Quat::Quat(Vec3 n, const float angleRadians)
{
    const float halfAngleRadians = 0.5f * angleRadians;
    W = cosf(halfAngleRadians);
    const float halfSine = sinf(halfAngleRadians);
    n.Normalize();
    X = n.X * halfSine;
    Y = n.Y * halfSine;
    Z = n.Z * halfSine;
}

const Quat& Quat::operator=(const Quat& rhs)
{
    X = rhs.X;
    Y = rhs.Y;
    Z = rhs.Z;
    W = rhs.W;
    return *this;
}

Quat& Quat::operator*=(const float& rhs)
{
    X *= rhs;
    Y *= rhs;
    Z *= rhs;
    W *= rhs;
    return *this;
}

Quat& Quat::operator*=(const Quat& rhs)
{
    Quat temp = *this * rhs;
    W = temp.W;
    X = temp.X;
    Y = temp.Y;
    Z = temp.Z;
    return *this;
}

Quat Quat::operator*(const Quat& rhs) const
{
    Quat temp;
    temp.W = (W * rhs.W) - (X * rhs.X) - (Y * rhs.Y) - (Z * rhs.Z);
    temp.X = (X * rhs.W) + (W * rhs.X) + (Y * rhs.Z) - (Z * rhs.Y);
    temp.Y = (Y * rhs.W) + (W * rhs.Y) + (Z * rhs.X) - (X * rhs.Z);
    temp.Z = (Z * rhs.W) + (W * rhs.Z) + (X * rhs.Y) - (Y * rhs.X);
    return temp;
}

void Quat::Normalize()
{
    float invMag = 1.0f / GetMagnitude();
    if (0.0f * invMag == 0.0f * invMag)
    {
        X = X * invMag;
        Y = Y * invMag;
        Z = Z * invMag;
        W = W * invMag;
    }
}

void Quat::Invert()
{
    *this *= 1.0f / MagnitudeSquared();
    X = -X;
    Y = -Y;
    Z = -Z;
}

Quat Quat::Inverse() const
{
    Quat val(*this);
    val.Invert();
    return val;
}

float Quat::MagnitudeSquared() const
{
    return ((X * X) + (Y * Y) + (Z * Z) + (W * W));
}

float Quat::GetMagnitude() const
{
    return sqrtf(MagnitudeSquared());
}

Vec3 Quat::RotatePoint(const Vec3& rhs) const
{
    Quat vector(rhs.X, rhs.Y, rhs.Z, 0.0f);
    Quat final = *this * vector * Inverse();
    return Vec3(final.X, final.Y, final.Z);
}

bool Quat::IsValid() const
{
    if (X * 0 != X * 0)
    {
        return false;
    }
    if (Y * 0 != Y * 0)
    {
        return false;
    }
    if (Z * 0 != Z * 0)
    {
        return false;
    }
    if (W * 0 != W * 0)
    {
        return false;
    }
    return true;
}

Mat3 Quat::RotateMatrix(const Mat3& rhs) const
{
    Mat3 mat;
    mat.Rows[0] = RotatePoint(rhs.Rows[0]);
    mat.Rows[1] = RotatePoint(rhs.Rows[1]);
    mat.Rows[2] = RotatePoint(rhs.Rows[2]);
    return mat;
}


/**
 * \brief Code referenced here https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/
 * \param roll 
 * \param pitch 
 * \param yaw 
 */
void Quat::GetEulerAngles(float& roll, float& pitch, float& yaw) const
{
    // Calculate roll (rotation around x)
    float t0 = +2.0f * (W * X + Y * Z);
    float t1 = +1.0f - 2.0f * (X * X + Y * Y);
    roll = std::atan2(t0, t1);

    // Calculate pitch (rotation around y)
    float t2 = +2.0f * (W * Y - Z * X);
    t2 = (t2 > +1.0f) ? +1.0f : t2;
    t2 = (t2 < -1.0f) ? -1.0f : t2;
    pitch = std::asin(t2);

    // Calculate yaw (rotation around z)
    float t3 = +2.0f * (W * Z + X * Y);
    float t4 = +1.0f - 2.0f * (Y * Y + Z * Z);
    yaw = std::atan2(t3, t4);
}

Mat3 Quat::ToMat3() const
{
    Mat3 mat;
    mat.Identity();
    mat.Rows[0] = RotatePoint(mat.Rows[0]);
    mat.Rows[1] = RotatePoint(mat.Rows[1]);
    mat.Rows[2] = RotatePoint(mat.Rows[2]);
    return mat;
}

Quat Quat::FromRotationMatrix(Mat3& m)
{
    double trace = m[0][0] + m[1][1] + m[2][2];
    Quat q;

    if (trace > 0) {
        float S = std::sqrt(trace + 1.0) * 2; // S=4*qw 
        q.W = 0.25 * S;
        q.X = (m[2][1] - m[1][2]) / S;
        q.Y = (m[0][2] - m[2][0]) / S;
        q.Z = (m[1][0] - m[0][1]) / S;
    }
    else if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) {
        float S = std::sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2; // S=4*qx 
        q.W = (m[2][1] - m[1][2]) / S;
        q.X = 0.25 * S;
        q.Y = (m[0][1] + m[1][0]) / S;
        q.Z = (m[0][2] + m[2][0]) / S;
    }
    else if (m[1][1] > m[2][2]) {
        float S = std::sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2; // S=4*qy
        q.W = (m[0][2] - m[2][0]) / S;
        q.X = (m[0][1] + m[1][0]) / S;
        q.Y = 0.25 * S;
        q.Z = (m[1][2] + m[2][1]) / S;
    }
    else {
        float S = std::sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2; // S=4*qz
        q.W = (m[1][0] - m[0][1]) / S;
        q.X = (m[0][2] + m[2][0]) / S;
        q.Y = (m[1][2] + m[2][1]) / S;
        q.Z = 0.25 * S;
    }
    return q;
}
