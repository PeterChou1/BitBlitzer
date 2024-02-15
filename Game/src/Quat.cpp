#include "stdafx.h"
#include "Quat.h"

#include <cassert>
#include <math.h>
#include <cmath>


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
    temp.W = W * rhs.W - X * rhs.X - Y * rhs.Y - Z * rhs.Z;
    temp.X = X * rhs.W + W * rhs.X + Y * rhs.Z - Z * rhs.Y;
    temp.Y = Y * rhs.W + W * rhs.Y + Z * rhs.X - X * rhs.Z;
    temp.Z = Z * rhs.W + W * rhs.Z + X * rhs.Y - Y * rhs.X;
    return temp;
}

void Quat::Normalize()
{
    float magnitude = GetMagnitude();
    assert(magnitude != 0.0);
    float invMag = 1.0f / magnitude;
    X = X * invMag;
    Y = Y * invMag;
    Z = Z * invMag;
    W = W * invMag;
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
    return X * X + Y * Y + Z * Z + W * W;
}

float Quat::GetMagnitude() const
{
    return sqrtf(MagnitudeSquared());
}

Vec3 Quat::RotatePoint(const Vec3& rhs) const
{
    Quat vector(rhs.X, rhs.Y, rhs.Z, 0.0f);
    Quat finalQuat = *this * vector * Inverse();
    return Vec3(finalQuat.X, finalQuat.Y, finalQuat.Z);
}

Mat3 Quat::RotateMatrix(const Mat3& rhs) const
{
    Mat3 mat;
    mat.Rows[0] = RotatePoint(rhs.Rows[0]);
    mat.Rows[1] = RotatePoint(rhs.Rows[1]);
    mat.Rows[2] = RotatePoint(rhs.Rows[2]);
    return mat;
}

/// Code Implementation Based On
/// https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/s
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
    float trace = m[0][0] + m[1][1] + m[2][2];
    Quat q;

    if (trace > 0) {
        float S = std::sqrtf(trace + 1.0f) * 2.0f; // S=4*qw 
        q.W = 0.25f * S;
        q.X = (m[2][1] - m[1][2]) / S;
        q.Y = (m[0][2] - m[2][0]) / S;
        q.Z = (m[1][0] - m[0][1]) / S;
    }
    else if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) {
        float S = std::sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f; // S=4*qx 
        q.W = (m[2][1] - m[1][2]) / S;
        q.X = 0.25f * S;
        q.Y = (m[0][1] + m[1][0]) / S;
        q.Z = (m[0][2] + m[2][0]) / S;
    }
    else if (m[1][1] > m[2][2]) {
        float S = std::sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f; // S=4*qy
        q.W = (m[0][2] - m[2][0]) / S;
        q.X = (m[0][1] + m[1][0]) / S;
        q.Y = 0.25f * S;
        q.Z = (m[1][2] + m[2][1]) / S;
    }
    else {
        float S = std::sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f; // S=4*qz
        q.W = (m[1][0] - m[0][1]) / S;
        q.X = (m[0][2] + m[2][0]) / S;
        q.Y = (m[1][2] + m[2][1]) / S;
        q.Z = 0.25f * S;
    }
    return q;
}

Quat Quat::Lerp(const Quat& quat1, const Quat& quat2, float t)
{
    Quat result;
    result.X = (1 - t) * quat1.X + t * quat2.X;
    result.Y = (1 - t) * quat1.Y + t * quat2.Y;
    result.Z = (1 - t) * quat1.Z + t * quat2.Z;
    result.W = (1 - t) * quat1.W + t * quat2.W;
    result.Normalize();
    return result;
}

Quat Quat::Slerp(const Quat& quat1, const Quat& quat2, float t)
{
    Quat q1 = quat1;
    Quat q2 = quat2;

    float dot = q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W;

    // If the dot is negative, invert one of the input quaternions
    // to take the shortest path
    if (dot < 0.0f) {
        q2 = Quat(-q2.X, -q2.Y, -q2.Z, -q2.W);
        dot = -dot;
    }

    // If the inputs are too close for comfort, linearly interpolate
    if (dot > 0.9995f) {
        return Lerp(q1, q2, t); // Use LERP for close quaternions
    }

    // Calculate coefficients
    float theta_0 = acos(dot); // theta_0 = angle between input quaternions
    float theta = theta_0 * t; // theta = angle between q1 and result
    float sin_theta = sin(theta); // compute this value only once
    float sin_theta_0 = sin(theta_0); // compute this value only once

    float s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    float s1 = sin_theta / sin_theta_0;

    return Quat(
        (s0 * q1.X) + (s1 * q2.X),
        (s0 * q1.Y) + (s1 * q2.Y),
        (s0 * q1.Z) + (s1 * q2.Z),
        (s0 * q1.W) + (s1 * q2.W)
    );
}
