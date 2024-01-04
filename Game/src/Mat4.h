#pragma once
#include "Vec4.h"
#include "Mat3.h"
/* ================================ Mat4 ================================ */

class Mat4
{
public:
    Mat4()
    {
    }

    Mat4(const Mat4& rhs);
    Mat4(const float* mat);
    Mat4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3);
    Mat4& operator=(const Mat4& rhs);

    ~Mat4()
    {
    }

    void Zero();
    void Identity();
    float Trace() const;
    float Determinant() const;
    Mat4 Transpose() const;
    Mat4 Inverse() const;
    // fast inverse specifically for affine matrix
    Mat4 AffineInverse() const;
    Mat3 Minor(int i, int j) const;
    float Cofactor(int i, int j) const;
    void Orient(Vec3 pos, Vec3 fwd, Vec3 up);
    void LookAt(Vec3 pos, Vec3 lookAt, Vec3 up);
    void PerspectiveOpenGL(float fovy, float aspect_ratio, float near, float far);
    const float* ToPtr() const { return rows[0].ToPtr(); }
    float* ToPtr() { return rows[0].ToPtr(); }
    Vec4 operator*(const Vec4& rhs) const;
    Vec3 operator*(const Vec3& rhs) const;
    Mat4 operator*(const float rhs) const;
    Mat4 operator*(const Mat4& rhs) const;
    Vec4 operator[](const int i) const;
    Vec4& operator[](const int i);

    const Mat4& operator*=(const float rhs);

    Vec4 rows[4];
};
