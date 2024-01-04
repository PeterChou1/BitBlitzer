#pragma once
#include <immintrin.h>
#include "Vec2.h"
#include "Vec3.h"
/* 
* /brief Float based on avx2
*/
class SIMDFloat
{
public:
    SIMDFloat() : m_avx(_mm256_setzero_ps())
    {
    }

    SIMDFloat(__m256 avx) : m_avx(avx)
    {
    }

    SIMDFloat(const float& a) : m_avx(_mm256_broadcast_ss(&a))
    {
    }

    SIMDFloat(float* a) : m_avx(_mm256_loadu_ps(a))
    {
    }

    __forceinline SIMDFloat operator*(const SIMDFloat& rhs) const
    {
        return _mm256_mul_ps(m_avx, rhs.m_avx);
    }

    __forceinline SIMDFloat operator*(const float rhs) const
    {
        return *this * SIMDFloat(rhs);
    }

    __forceinline SIMDFloat operator+(const SIMDFloat& rhs) const
    {
        return _mm256_add_ps(m_avx, rhs.m_avx);
    }

    __forceinline SIMDFloat operator-(const SIMDFloat& rhs) const
    {
        return _mm256_sub_ps(m_avx, rhs.m_avx);
    }

    __forceinline float& operator[](const size_t i)
    {
        return v[i];
    }

    __forceinline bool GetBit(const size_t i) const
    {
        return (_mm256_movemask_ps(m_avx) >> i) & 1;
    }

    __forceinline SIMDFloat operator>=(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLT_US);
    }

    __forceinline SIMDFloat operator>=(const float rhs) const
    {
        return *this >= SIMDFloat(rhs);
    }

    __forceinline SIMDFloat operator>(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLE_US);
    }

    __forceinline SIMDFloat operator>(const float rhs) const
    {
        return *this > SIMDFloat(rhs);
    }

    __forceinline SIMDFloat operator<(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LT_OS);
    }

    __forceinline SIMDFloat operator<(const float rhs) const
    {
        return *this < SIMDFloat(rhs);
    }

    __forceinline SIMDFloat operator<=(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LE_OS);
    }

    __forceinline SIMDFloat operator<=(const float rhs) const
    {
        return *this <= SIMDFloat(rhs);
    }

    __forceinline SIMDFloat operator&(const SIMDFloat& rhs) const
    {
        return _mm256_and_ps(m_avx, rhs.m_avx);
    }

    __forceinline SIMDFloat operator|(const SIMDFloat& rhs) const
    {
        return _mm256_or_ps(m_avx, rhs.m_avx);
    }

    __forceinline SIMDFloat operator==(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_EQ_US);
    }

    __forceinline SIMDFloat operator/(const SIMDFloat& rhs) const
    {
        return SIMDFloat(m_avx) * SIMDFloat(_mm256_rcp_ps(rhs.m_avx));
    }

    __forceinline SIMDFloat operator/(const float rhs) const
    {
        return *this * (1.0f / rhs);
    }

    union
    {
        __m256 m_avx;
        float v[8];
    };
};


class SIMDVec2
{
public:
    SIMDVec2() = default;

    SIMDVec2(float x, float y) : x(SIMDFloat(x)), y(SIMDFloat(y))
    {
    }

    SIMDVec2(SIMDFloat x, SIMDFloat y) : x(x), y(y)
    {
    }

    SIMDVec2(Vec2 v) : x(v.x), y(v.y) {}


    __forceinline SIMDVec2 operator+(const SIMDVec2& rhs) const
    {
        return {x + rhs.x, y + rhs.y};
    }

    __forceinline SIMDVec2 operator*(const SIMDFloat& rhs) const
    {
        return {x * rhs, y * rhs};
    }

    __forceinline SIMDVec2 operator/(const SIMDFloat& rhs) const
    {
        return {x / rhs, y / rhs};
    }

    SIMDFloat x{}, y{};
};

class SIMDVec3
{
public:
    SIMDVec3() = default;

    SIMDVec3(float x, float y, float z) : x(x), y(y), z(z)
    {
    }

    SIMDVec3(SIMDFloat x, SIMDFloat y, SIMDFloat z) : x(x), y(y), z(z)
    {
    }

    SIMDVec3(Vec3 v) : x(v.x), y(v.y), z(v.z) {}

    __forceinline SIMDVec3 operator+(const SIMDVec3& rhs) const
    {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    __forceinline SIMDVec3 operator*(const SIMDFloat& rhs) const
    {
        return {x * rhs, y * rhs, z * rhs};
    }

    __forceinline SIMDVec3 operator/(const SIMDFloat& rhs) const
    {
        return {x / rhs, y / rhs, z / rhs};
    }

    SIMDFloat x{}, y{}, z{};
};


namespace SIMD
{
    __forceinline bool All(const SIMDFloat& rhs)
    {
        return _mm256_movemask_ps(rhs.m_avx) == 255;
    };

    __forceinline bool Any(const SIMDFloat& rhs)
    {
        return _mm256_movemask_ps(rhs.m_avx) != 0x0;
    };

    // if mask bit == 0 than A bit will be chosen
    // if mask bit == 1 than B bit will be chosen
    __forceinline SIMDFloat Select(const SIMDFloat& mask, SIMDFloat& a, SIMDFloat& b)
    {
        return _mm256_blendv_ps(a.m_avx, b.m_avx, mask.m_avx);
    }
};
