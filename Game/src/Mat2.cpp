#include "stdafx.h"
#include "Mat2.h"
#include <math.h>
#include <cassert>


Mat2::Mat2(const Mat2& rhs)
{
    Rows[0] = rhs.Rows[0];
    Rows[1] = rhs.Rows[1];
}

Mat2::Mat2(const float* mat)
{
    Rows[0] = mat + 0;
    Rows[1] = mat + 2;
}

Mat2::Mat2(const Vec2& row0, const Vec2& row1)
{
    Rows[0] = row0;
    Rows[1] = row1;
}

Vec2 Mat2::operator*(const Vec2& rhs)
{
    Vec2 tmp;
    tmp[0] = Rows[0].Dot(rhs);
    tmp[1] = Rows[1].Dot(rhs);
    return tmp;
}

Mat2& Mat2::operator=(const Mat2& rhs)
{
    Rows[0] = rhs.Rows[0];
    Rows[1] = rhs.Rows[1];
    return *this;
}

const Mat2& Mat2::operator*=(const float rhs)
{
    Rows[0] *= rhs;
    Rows[1] *= rhs;
    return *this;
}

const Mat2& Mat2::operator+=(const Mat2& rhs)
{
    Rows[0] += rhs.Rows[0];
    Rows[1] += rhs.Rows[1];
    return *this;
}

Vec2 Mat2::operator[](const int i) const
{
    assert(i >= 0 && i < 2);
    return Rows[i];
}

Vec2 Mat2::operator[](const int i)
{
    assert(i >= 0 && i < 2);
    return Rows[i];
}
