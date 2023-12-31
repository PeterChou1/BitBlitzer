#pragma once
#include "Vec3.h"
#include "Mat2.h"


class Quat;

class Mat3 {
public:
    Mat3() {}
    Mat3(const Mat3& rhs);
    Mat3(const float* mat);
    Mat3(const Vec3& row0, const Vec3& row1, const Vec3& row2);
    Mat3& operator=(const Mat3& rhs);
    void Zero();
    void Identity();
    float Trace() const;
    float Determinant() const;
    Mat3 Transpose() const;
    Mat3 Inverse() const;
    Mat2 Minor(const int i, const int j) const;
    float CoFactor(const int i, const int j) const;
    Vec3 operator*(const Vec3& rhs) const;
    Mat3 operator*(const float rhs) const;
    Mat3 operator*(const Mat3& rhs) const;
    Mat3 operator+(const Mat3& rhs) const;
    const Mat3& operator*=(const float rhs);
    const Mat3& operator+=(const Mat3& rhs);
    Vec3 operator[](const int i) const;
    Vec3& operator[](const int i);

    static Mat3 FromQuat(const Quat& q);

public:
    Vec3 rows[3];
};