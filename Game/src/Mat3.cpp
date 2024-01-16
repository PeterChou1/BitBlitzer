#include "stdafx.h"
#include "Mat3.h"
#include "Quat.h"
#include <math.h>
#include <cassert>

Mat3::Mat3(const Mat3& rhs)
{
    Rows[0] = rhs.Rows[0];
    Rows[1] = rhs.Rows[1];
    Rows[2] = rhs.Rows[2];
}

Mat3::Mat3(const float* mat)
{
    Rows[0] = mat + 0;
    Rows[1] = mat + 3;
    Rows[2] = mat + 6;
}

Mat3::Mat3(const Vec3& row0, const Vec3& row1, const Vec3& row2)
{
    Rows[0] = row0;
    Rows[1] = row1;
    Rows[2] = row2;
}


Mat3& Mat3::operator=(const Mat3& rhs)
{
    Rows[0] = rhs.Rows[0];
    Rows[1] = rhs.Rows[1];
    Rows[2] = rhs.Rows[2];
    return *this;
}

const Mat3& Mat3::operator*=(const float rhs)
{
    Rows[0] *= rhs;
    Rows[1] *= rhs;
    Rows[2] *= rhs;
    return *this;
}

const Mat3& Mat3::operator+=(const Mat3& rhs)
{
    Rows[0] += rhs.Rows[0];
    Rows[1] += rhs.Rows[1];
    Rows[2] += rhs.Rows[2];
    return *this;
}

Vec3 Mat3::operator[](const int i) const
{
    assert(i >= 0 && i < 3);
    return Rows[i];
}

Vec3& Mat3::operator[](const int i)
{
    assert(i >= 0 && i < 3);
    return Rows[i];
}

Mat3 Mat3::FromQuat(const Quat& q)
{
    float x = q.X;
    float y = q.Y;
    float z = q.Z;
    float w = q.W;

    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xw = x * w;

    float yy = y * y;
    float yz = y * z;
    float yw = y * w;

    float zz = z * z;
    float zw = z * w;

    Mat3 m;

    m[0][0] = 1.0f - 2.0f * (yy + zz);
    m[0][1] = 2 * (xy - zw);
    m[0][2] = 2 * (xz + yw);

    m[1][0] = 2 * (xy + zw);
    m[1][1] = 1 - 2 * (xx + zz);
    m[1][2] = 2 * (yz - xw);


    m[2][0] = 2 * (xz - yw);
    m[2][1] = 2 * (yz + xw);
    m[2][2] = 1 - 2 * (xx + yy);

    return m;
}


void Mat3::Zero()
{
    Rows[0].Zero();
    Rows[1].Zero();
    Rows[2].Zero();
}

void Mat3::Identity()
{
    Rows[0] = Vec3(1, 0, 0);
    Rows[1] = Vec3(0, 1, 0);
    Rows[2] = Vec3(0, 0, 1);
}

float Mat3::Trace() const
{
    const float xx = Rows[0][0] * Rows[0][0];
    const float yy = Rows[1][1] * Rows[1][1];
    const float zz = Rows[2][2] * Rows[2][2];
    return (xx + yy + zz);
}

float Mat3::Determinant() const
{
    const float i = Rows[0][0] * (Rows[1][1] * Rows[2][2] - Rows[1][2] * Rows[2][1]);
    const float j = Rows[0][1] * (Rows[1][0] * Rows[2][2] - Rows[1][2] * Rows[2][0]);
    const float k = Rows[0][2] * (Rows[1][0] * Rows[2][1] - Rows[1][1] * Rows[2][0]);
    return (i - j + k);
}

Mat3 Mat3::Transpose() const
{
    Mat3 transpose;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            transpose.Rows[i][j] = Rows[j][i];
        }
    }
    return transpose;
}

Mat3 Mat3::Inverse() const
{
    Mat3 inv;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            inv.Rows[j][i] = CoFactor(i, j);
        }
    }
    const float det = Determinant();
    const float invDet = 1.0f / det;
    inv *= invDet;
    return inv;
}

Mat2 Mat3::Minor(const int i, const int j) const
{
    Mat2 minor;
    int yy = 0;
    for (int y = 0; y < 3; y++)
    {
        if (y == j)
        {
            continue;
        }
        int xx = 0;
        for (int x = 0; x < 3; x++)
        {
            if (x == i)
            {
                continue;
            }
            minor.Rows[xx][yy] = Rows[x][y];
            xx++;
        }
        yy++;
    }
    return minor;
}

float Mat3::CoFactor(const int i, const int j) const
{
    const Mat2 minor = Minor(i, j);
    const float C = float(pow(-1, i + 1 + j + 1)) * minor.Determinant();
    return C;
}

Vec3 Mat3::operator*(const Vec3& rhs) const
{
    Vec3 tmp;
    tmp[0] = Rows[0].Dot(rhs);
    tmp[1] = Rows[1].Dot(rhs);
    tmp[2] = Rows[2].Dot(rhs);
    return tmp;
}

Mat3 Mat3::operator*(const float rhs) const
{
    Mat3 tmp;
    tmp.Rows[0] = Rows[0] * rhs;
    tmp.Rows[1] = Rows[1] * rhs;
    tmp.Rows[2] = Rows[2] * rhs;
    return tmp;
}

Mat3 Mat3::operator*(const Mat3& rhs) const
{
    Mat3 tmp;
    for (int i = 0; i < 3; i++)
    {
        tmp.Rows[i].X = Rows[i].X * rhs.Rows[0].X + Rows[i].Y * rhs.Rows[1].X + Rows[i].Z * rhs.Rows[2].X;
        tmp.Rows[i].Y = Rows[i].X * rhs.Rows[0].Y + Rows[i].Y * rhs.Rows[1].Y + Rows[i].Z * rhs.Rows[2].Y;
        tmp.Rows[i].Z = Rows[i].X * rhs.Rows[0].Z + Rows[i].Y * rhs.Rows[1].Z + Rows[i].Z * rhs.Rows[2].Z;
    }
    return tmp;
}

Mat3 Mat3::operator+(const Mat3& rhs) const
{
    Mat3 tmp;
    for (int i = 0; i < 3; i++)
    {
        tmp.Rows[i] = Rows[i] + rhs.Rows[i];
    }
    return tmp;
}
