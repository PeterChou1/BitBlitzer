#include "stdafx.h"
#include "Mat2.h"
#include <math.h>
#include <cassert>


Mat2::Mat2(const Mat2& rhs)
{
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
}

Mat2::Mat2(const float* mat)
{
    rows[0] = mat + 0;
    rows[1] = mat + 2;
}

Mat2::Mat2(const Vec2& row0, const Vec2& row1)
{
    rows[0] = row0;
    rows[1] = row1;
}

Vec2 Mat2::operator*(const Vec2& rhs)
{
    Vec2 tmp;
    tmp[0] = rows[0].Dot(rhs);
    tmp[1] = rows[1].Dot(rhs);
    return tmp;
}

Mat2& Mat2::operator=(const Mat2& rhs)
{
    rows[0] = rhs.rows[0];
    rows[1] = rhs.rows[1];
    return *this;
}

const Mat2& Mat2::operator*=(const float rhs)
{
    rows[0] *= rhs;
    rows[1] *= rhs;
    return *this;
}

const Mat2& Mat2::operator+=(const Mat2& rhs)
{
    rows[0] += rhs.rows[0];
    rows[1] += rhs.rows[1];
    return *this;
}

Vec2 Mat2::operator[](const int i) const
{
    assert(i >= 0 && i < 2);
    return rows[i];
}

Vec2 Mat2::operator[](const int i)
{
    assert(i >= 0 && i < 2);
    return rows[i];
}
