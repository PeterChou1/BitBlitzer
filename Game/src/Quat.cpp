#include "stdafx.h"
#include "Quat.h"
#include <math.h>
#include <cmath>;

Quat::Quat() : x(0), y(0), z(0), w(1)
{
}

Quat::Quat(const Quat& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}

Quat::Quat(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W)
{
}

Quat::Quat(Vec3 n, const float angleRadians)
{
    const float halfAngleRadians = 0.5f * angleRadians;
    w = cosf(halfAngleRadians);
    const float halfSine = sinf(halfAngleRadians);
    n.Normalize();
    x = n.x * halfSine;
    y = n.y * halfSine;
    z = n.z * halfSine;
}

const Quat& Quat::operator=(const Quat& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

Quat& Quat::operator*=(const float& rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

Quat& Quat::operator*=(const Quat& rhs)
{
    Quat temp = *this * rhs;
    w = temp.w;
    x = temp.x;
    y = temp.y;
    z = temp.z;
    return *this;
}

Quat Quat::operator*(const Quat& rhs) const
{
    Quat temp;
    temp.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
    temp.x = (x * rhs.w) + (w * rhs.x) + (y * rhs.z) - (z * rhs.y);
    temp.y = (y * rhs.w) + (w * rhs.y) + (z * rhs.x) - (x * rhs.z);
    temp.z = (z * rhs.w) + (w * rhs.z) + (x * rhs.y) - (y * rhs.x);
    return temp;
}

void Quat::Normalize()
{
    float invMag = 1.0f / GetMagnitude();
    if (0.0f * invMag == 0.0f * invMag)
    {
        x = x * invMag;
        y = y * invMag;
        z = z * invMag;
        w = w * invMag;
    }
}

void Quat::Invert()
{
    *this *= 1.0f / MagnitudeSquared();
    x = -x;
    y = -y;
    z = -z;
}

Quat Quat::Inverse() const
{
    Quat val(*this);
    val.Invert();
    return val;
}

float Quat::MagnitudeSquared() const
{
    return ((x * x) + (y * y) + (z * z) + (w * w));
}

float Quat::GetMagnitude() const
{
    return sqrtf(MagnitudeSquared());
}

Vec3 Quat::RotatePoint(const Vec3& rhs) const
{
    Quat vector(rhs.x, rhs.y, rhs.z, 0.0f);
    Quat final = *this * vector * Inverse();
    return Vec3(final.x, final.y, final.z);
}

bool Quat::IsValid() const
{
    if (x * 0 != x * 0)
    {
        return false;
    }
    if (y * 0 != y * 0)
    {
        return false;
    }
    if (z * 0 != z * 0)
    {
        return false;
    }
    if (w * 0 != w * 0)
    {
        return false;
    }
    return true;
}

Mat3 Quat::RotateMatrix(const Mat3& rhs) const
{
    Mat3 mat;
    mat.rows[0] = RotatePoint(rhs.rows[0]);
    mat.rows[1] = RotatePoint(rhs.rows[1]);
    mat.rows[2] = RotatePoint(rhs.rows[2]);
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
    float t0 = +2.0f * (w * x + y * z);
    float t1 = +1.0f - 2.0f * (x * x + y * y);
    roll = std::atan2(t0, t1);

    // Calculate pitch (rotation around y)
    float t2 = +2.0f * (w * y - z * x);
    t2 = (t2 > +1.0f) ? +1.0f : t2;
    t2 = (t2 < -1.0f) ? -1.0f : t2;
    pitch = std::asin(t2);

    // Calculate yaw (rotation around z)
    float t3 = +2.0f * (w * z + x * y);
    float t4 = +1.0f - 2.0f * (y * y + z * z);
    yaw = std::atan2(t3, t4);
}

Mat3 Quat::ToMat3() const
{
    Mat3 mat;
    mat.Identity();
    mat.rows[0] = RotatePoint(mat.rows[0]);
    mat.rows[1] = RotatePoint(mat.rows[1]);
    mat.rows[2] = RotatePoint(mat.rows[2]);
    return mat;
}

Quat Quat::FromRotationMatrix(Mat3& m)
{
    double trace = m[0][0] + m[1][1] + m[2][2];
    Quat q;

    if (trace > 0) {
        float S = std::sqrt(trace + 1.0) * 2; // S=4*qw 
        q.w = 0.25 * S;
        q.x = (m[2][1] - m[1][2]) / S;
        q.y = (m[0][2] - m[2][0]) / S;
        q.z = (m[1][0] - m[0][1]) / S;
    }
    else if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) {
        float S = std::sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2; // S=4*qx 
        q.w = (m[2][1] - m[1][2]) / S;
        q.x = 0.25 * S;
        q.y = (m[0][1] + m[1][0]) / S;
        q.z = (m[0][2] + m[2][0]) / S;
    }
    else if (m[1][1] > m[2][2]) {
        float S = std::sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2; // S=4*qy
        q.w = (m[0][2] - m[2][0]) / S;
        q.x = (m[0][1] + m[1][0]) / S;
        q.y = 0.25 * S;
        q.z = (m[1][2] + m[2][1]) / S;
    }
    else {
        float S = std::sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2; // S=4*qz
        q.w = (m[1][0] - m[0][1]) / S;
        q.x = (m[0][2] + m[2][0]) / S;
        q.y = (m[1][2] + m[2][1]) / S;
        q.z = 0.25 * S;
    }
    return q;
}
