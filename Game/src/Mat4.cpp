#include "stdafx.h"
#include "Mat4.h"
#include <math.h>
#include <cassert>
#include <immintrin.h>  // For AVX-512

Mat4::Mat4(const Mat4& rhs)
{
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
    rows[2] = rhs.rows[2];
    rows[3] = rhs.rows[3];
}

Mat4::Mat4(const float* mat)
{
    rows[0] = mat + 0;
    rows[1] = mat + 4;
    rows[2] = mat + 8;
    rows[3] = mat + 12;
}

Mat4::Mat4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3)
{
    rows[0] = row0;
    rows[1] = row1;
    rows[2] = row2;
    rows[3] = row3;
}

Mat4& Mat4::operator=(const Mat4& rhs)
{
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
    rows[2] = rhs.rows[2];
    rows[3] = rhs.rows[3];
    return *this;
}

const Mat4& Mat4::operator*=(const float rhs)
{
    rows[0] *= rhs;
    rows[1] *= rhs;
    rows[2] *= rhs;
    rows[3] *= rhs;
    return *this;
}

void Mat4::Zero()
{
    rows[0].Zero();
    rows[1].Zero();
    rows[2].Zero();
    rows[3].Zero();
}

void Mat4::Identity()
{
    rows[0] = Vec4(1, 0, 0, 0);
    rows[1] = Vec4(0, 1, 0, 0);
    rows[2] = Vec4(0, 0, 1, 0);
    rows[3] = Vec4(0, 0, 0, 1);
}

float Mat4::Trace() const
{
    const float xx = rows[0][0] * rows[0][0];
    const float yy = rows[1][1] * rows[1][1];
    const float zz = rows[2][2] * rows[2][2];
    const float ww = rows[3][3] * rows[3][3];
    return (xx + yy + zz + ww);
}

float Mat4::Determinant() const
{
    float det = 0.0f;
    float sign = 1.0f;
    for (int j = 0; j < 4; j++)
    {
        Mat3 minor = Minor(0, j);
        det += rows[0][j] * minor.Determinant() * sign;
        sign = -sign;
    }
    return det;
}

Mat4 Mat4::Transpose() const
{
    Mat4 transpose;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            transpose.rows[i][j] = rows[j][i];
        }
    }
    return transpose;
}

Mat4 Mat4::Inverse() const
{
    Mat4 inv;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            inv.rows[j][i] = Cofactor(i, j);
        }
    }
    float det = Determinant();
    float invDet = 1.0f / det;
    inv *= invDet;
    return inv;
}

Mat4 Mat4::AffineInverse() const
{
    Mat4 matrix;
    matrix[0][0] = (*this)[0][0];
    matrix[1][0] = (*this)[0][1];
    matrix[2][0] = (*this)[0][2];
    matrix[3][0] = 0.0f;
    matrix[0][1] = (*this)[1][0];
    matrix[1][1] = (*this)[1][1];
    matrix[2][1] = (*this)[1][2];
    matrix[3][1] = 0.0f;
    matrix[0][2] = (*this)[2][0];
    matrix[1][2] = (*this)[2][1];
    matrix[2][2] = (*this)[2][2];
    matrix[3][2] = 0.0f;
    matrix[0][3] = -((*this)[0][3] * matrix[0][0] + (*this)[1][3] * matrix[0][1] + (*this)[2][3] * matrix[0][2]);
    matrix[1][3] = -((*this)[0][3] * matrix[1][0] + (*this)[1][3] * matrix[1][1] + (*this)[2][3] * matrix[1][2]);
    matrix[2][3] = -((*this)[0][3] * matrix[2][0] + (*this)[1][3] * matrix[2][1] + (*this)[2][3] * matrix[2][2]);
    matrix[3][3] = 1.0f;
    return matrix;
}

Mat3 Mat4::Minor(const int i, const int j) const
{
    Mat3 minor;
    int yy = 0;
    for (int y = 0; y < 4; y++)
    {
        if (y == j)
        {
            continue;
        }
        int xx = 0;
        for (int x = 0; x < 4; x++)
        {
            if (x == i)
            {
                continue;
            }
            minor.rows[xx][yy] = rows[x][y];
            xx++;
        }
        yy++;
    }
    return minor;
}

float Mat4::Cofactor(const int i, const int j) const
{
    const Mat3 minor = Minor(i, j);
    const float C = float(pow(-1, i + 1 + j + 1)) * minor.Determinant();
    return C;
}

void Mat4::Orient(Vec3 pos, Vec3 fwd, Vec3 up)
{
    Vec3 left = up.Cross(fwd);

    // For our coordinate system where:
    // +x-axis = fwd
    // +y-axis = left
    // +z-axis = up
    rows[0] = Vec4(fwd.x, left.x, up.x, pos.x);
    rows[1] = Vec4(fwd.y, left.y, up.y, pos.y);
    rows[2] = Vec4(fwd.z, left.z, up.z, pos.z);
    rows[3] = Vec4(0, 0, 0, 1);
}

void Mat4::LookAt(Vec3 pos, Vec3 lookAt, Vec3 up)
{
    Vec3 fwd = pos - lookAt;
    fwd.Normalize();
    Vec3 right = up.Cross(fwd);
    right.Normalize();
    up = fwd.Cross(right);
    up.Normalize();

    // For NDC coordinate system where:
    // +x-axis = right
    // +y-axis = up
    // +z-axis = fwd
    rows[0] = Vec4(right.x, right.y, right.z, -pos.Dot(right));
    rows[1] = Vec4(up.x, up.y, up.z, -pos.Dot(up));
    rows[2] = Vec4(fwd.x, fwd.y, fwd.z, -pos.Dot(fwd));
    rows[3] = Vec4(0, 0, 0, 1);
}

void Mat4::PerspectiveOpenGL(float fovy, float aspect_ratio, float near, float far)
{
    const float pi = acosf(-1.0f);
    const double scale = tan(fovy * 0.5 * pi / 180.0) * near;
    double right = aspect_ratio * scale;
    double left = -right;
    double top = scale;
    double bottom = -top;

    rows[0] = Vec4(2 * near / (right - left), 0, (right + left) / (right - left), 0);
    rows[1] = Vec4(0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0);
    rows[2] = Vec4(0, 0, -(far + near) / (far - near), (-2.0f * far * near) / (far - near));
    rows[3] = Vec4(0, 0, -1, 0);
}

void Mat4::PerspectiveOLC(float fovy, float aspect_ratio, float far, float near)
{
    float fFovRad = 1.0f / tanf(fovy * 0.5f / 180.0f * 3.14159f);
    (*this)[0][0] = aspect_ratio * fFovRad;
    (*this)[1][1] = fFovRad;
    (*this)[2][2] = far / (far - near);
    (*this)[2][3] = (-far * near) / (far - near);
    (*this)[3][2] = 1.0f;
    (*this)[3][3] = 0.0f;
}

void Mat4::PerspectiveVulkan(float fovy, float aspect_ratio, float near, float far)
{
    // Vulkan changed its NDC. It switched from a left-handed
    // coordinate system to a right-handed one.
    // +x points to the right,
    // +z points into the screen,
    // +y points down (it used to point up, in OpenGL).
    // It also changed the range from [-1, 1] to [0, 1] for the z.
    // Clip space remains [-1, 1] for x and y.
    // Check section 23 of the specification.
    Mat4 matVulkan;
    matVulkan.rows[0] = Vec4(1, 0, 0, 0);
    matVulkan.rows[1] = Vec4(0, -1, 0, 0);
    matVulkan.rows[2] = Vec4(0, 0, 0.5f, 0.5f);
    matVulkan.rows[3] = Vec4(0, 0, 0, 1);

    Mat4 matOpenGL;
    matOpenGL.PerspectiveOpenGL(fovy, aspect_ratio, near, far);
    *this = matVulkan * matOpenGL;
}

Vec4 Mat4::operator*(const Vec4& rhs) const
{
    Vec4 tmp;
    tmp[0] = rows[0].Dot(rhs);
    tmp[1] = rows[1].Dot(rhs);
    tmp[2] = rows[2].Dot(rhs);
    tmp[3] = rows[3].Dot(rhs);
    return tmp;
}

Vec3 Mat4::operator*(const Vec3& rhs) const
{
    return (*this * Vec4(rhs)).ToVec3();
}


Mat4 Mat4::operator*(const float rhs) const
{
    Mat4 tmp;
    tmp.rows[0] = rows[0] * rhs;
    tmp.rows[1] = rows[1] * rhs;
    tmp.rows[2] = rows[2] * rhs;
    tmp.rows[3] = rows[3] * rhs;
    return tmp;
}

Mat4 Mat4::operator*(const Mat4& rhs) const
{
    Mat4 tmp;
    for (int i = 0; i < 4; i++)
    {
        tmp.rows[i].x = rows[i].x * rhs.rows[0].x + rows[i].y * rhs.rows[1].x + rows[i].z * rhs.rows[2].x + rows[i].w *
            rhs.rows[3].x;
        tmp.rows[i].y = rows[i].x * rhs.rows[0].y + rows[i].y * rhs.rows[1].y + rows[i].z * rhs.rows[2].y + rows[i].w *
            rhs.rows[3].y;
        tmp.rows[i].z = rows[i].x * rhs.rows[0].z + rows[i].y * rhs.rows[1].z + rows[i].z * rhs.rows[2].z + rows[i].w *
            rhs.rows[3].z;
        tmp.rows[i].w = rows[i].x * rhs.rows[0].w + rows[i].y * rhs.rows[1].w + rows[i].z * rhs.rows[2].w + rows[i].w *
            rhs.rows[3].w;
    }
    return tmp;
}

Vec4 Mat4::operator[](const int i) const
{
    assert(i >= 0 && i < 4);
    return rows[i];
}

Vec4& Mat4::operator[](const int i)
{
    assert(i >= 0 && i < 4);
    return rows[i];
}
