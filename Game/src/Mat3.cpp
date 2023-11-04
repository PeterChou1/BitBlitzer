#include "stdafx.h"
#include "Mat3.h"
#include <math.h>
#include <cassert>

Mat3::Mat3(const Mat3& rhs) {
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
    rows[2] = rhs.rows[2];
}

Mat3::Mat3(const float* mat) {
    rows[0] = mat + 0;
    rows[1] = mat + 3;
    rows[2] = mat + 6;
}

Mat3::Mat3(const Vec3& row0, const Vec3& row1, const Vec3& row2) {
    rows[0] = row0;
    rows[1] = row1;
    rows[2] = row2;
}


Mat3& Mat3::operator=(const Mat3& rhs) {
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
    rows[2] = rhs.rows[2];
    return *this;
}

const Mat3& Mat3::operator*=(const float rhs) {
    rows[0] *= rhs;
    rows[1] *= rhs;
    rows[2] *= rhs;
    return *this;
}

const Mat3& Mat3::operator+=(const Mat3& rhs) {
    rows[0] += rhs.rows[0];
    rows[1] += rhs.rows[1];
    rows[2] += rhs.rows[2];
    return *this;
}

Vec3 Mat3::operator[](const int i) const
{
    assert(i >= 0 && i < 3);
    return rows[i];
}

Vec3 Mat3::operator[](const int i)
{
    assert(i >= 0 && i < 3);
    return rows[i];
}

void Mat3::Zero() {
    rows[0].Zero();
    rows[1].Zero();
    rows[2].Zero();
}

void Mat3::Identity() {
    rows[0] = Vec3(1, 0, 0);
    rows[1] = Vec3(0, 1, 0);
    rows[2] = Vec3(0, 0, 1);
}

float Mat3::Trace() const {
    const float xx = rows[0][0] * rows[0][0];
    const float yy = rows[1][1] * rows[1][1];
    const float zz = rows[2][2] * rows[2][2];
    return (xx + yy + zz);
}

float Mat3::Determinant() const {
    const float i = rows[0][0] * (rows[1][1] * rows[2][2] - rows[1][2] * rows[2][1]);
    const float j = rows[0][1] * (rows[1][0] * rows[2][2] - rows[1][2] * rows[2][0]);
    const float k = rows[0][2] * (rows[1][0] * rows[2][1] - rows[1][1] * rows[2][0]);
    return (i - j + k);
}

Mat3 Mat3::Transpose() const {
    Mat3 transpose;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transpose.rows[i][j] = rows[j][i];
        }
    }
    return transpose;
}

Mat3 Mat3::Inverse() const {
    Mat3 inv;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            inv.rows[j][i] = CoFactor(i, j);
        }
    }
    const float det = Determinant();
    const float invDet = 1.0f / det;
    inv *= invDet;
    return inv;
}

Mat2 Mat3::Minor(const int i, const int j) const {
    Mat2 minor;
    int yy = 0;
    for (int y = 0; y < 3; y++) {
        if (y == j) {
            continue;
        }
        int xx = 0;
        for (int x = 0; x < 3; x++) {
            if (x == i) {
                continue;
            }
            minor.rows[xx][yy] = rows[x][y];
            xx++;
        }
        yy++;
    }
    return minor;
}

float Mat3::CoFactor(const int i, const int j) const {
    const Mat2 minor = Minor(i, j);
    const float C = float(pow(-1, i + 1 + j + 1)) * minor.Determinant();
    return C;
}

Vec3 Mat3::operator*(const Vec3& rhs) const {
    Vec3 tmp;
    tmp[0] = rows[0].Dot(rhs);
    tmp[1] = rows[1].Dot(rhs);
    tmp[2] = rows[2].Dot(rhs);
    return tmp;
}

Mat3 Mat3::operator*(const float rhs) const {
    Mat3 tmp;
    tmp.rows[0] = rows[0] * rhs;
    tmp.rows[1] = rows[1] * rhs;
    tmp.rows[2] = rows[2] * rhs;
    return tmp;
}

Mat3 Mat3::operator*(const Mat3& rhs) const {
    Mat3 tmp;
    for (int i = 0; i < 3; i++) {
        tmp.rows[i].x = rows[i].x * rhs.rows[0].x + rows[i].y * rhs.rows[1].x + rows[i].z * rhs.rows[2].x;
        tmp.rows[i].y = rows[i].x * rhs.rows[0].y + rows[i].y * rhs.rows[1].y + rows[i].z * rhs.rows[2].y;
        tmp.rows[i].z = rows[i].x * rhs.rows[0].z + rows[i].y * rhs.rows[1].z + rows[i].z * rhs.rows[2].z;
    }
    return tmp;
}

Mat3 Mat3::operator+(const Mat3& rhs) const {
    Mat3 tmp;
    for (int i = 0; i < 3; i++) {
        tmp.rows[i] = rows[i] + rhs.rows[i];
    }
    return tmp;
}