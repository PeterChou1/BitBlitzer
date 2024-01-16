#pragma once
#include "Vec2.h"


class Mat2
{
public:
    Mat2()
    {
    }

    Mat2(const Mat2& rhs);
    Mat2(const float* mat);
    Mat2(const Vec2& row0, const Vec2& row1);

    Vec2 operator*(const Vec2& rhs);
    Mat2& operator=(const Mat2& rhs);
    const Mat2& operator*=(const float rhs);
    const Mat2& operator+=(const Mat2& rhs);
    Vec2 operator[](const int i) const;
    Vec2 operator[](const int i);

    float Determinant() const
    {
        return Rows[0].X * Rows[1].Y - Rows[0].Y * Rows[1].X;
    }


    Vec2 Rows[2];
};
