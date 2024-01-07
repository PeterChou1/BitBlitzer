#pragma once
#include <immintrin.h>
#include "Vec2.h"
#include "Vec3.h"
#include <cmath>

// -- uncomment this if your processor does not support AVX2 --
#define SIMD

#ifdef SIMD
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

    SIMDFloat operator*(const SIMDFloat& rhs) const
    {
        return _mm256_mul_ps(m_avx, rhs.m_avx);
    }

    SIMDFloat operator*(const float rhs) const
    {
        return *this * SIMDFloat(rhs);
    }

    SIMDFloat operator+(const SIMDFloat& rhs) const
    {
        return _mm256_add_ps(m_avx, rhs.m_avx);
    }

    SIMDFloat operator-(const SIMDFloat& rhs) const
    {
        return _mm256_sub_ps(m_avx, rhs.m_avx);
    }

    float& operator[](const size_t i)
    {
        return v[i];
    }

    bool GetBit(const size_t i) const
    {
        return (_mm256_movemask_ps(m_avx) >> i) & 1;
    }

    SIMDFloat operator>=(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLT_US);
    }

    SIMDFloat operator>=(const float rhs) const
    {
        return *this >= SIMDFloat(rhs);
    }

    SIMDFloat operator>(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLE_US);
    }

    SIMDFloat operator>(const float rhs) const
    {
        return *this > SIMDFloat(rhs);
    }

    SIMDFloat operator<(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LT_OS);
    }

    SIMDFloat operator<(const float rhs) const
    {
        return *this < SIMDFloat(rhs);
    }

    SIMDFloat operator<=(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LE_OS);
    }

    SIMDFloat operator<=(const float rhs) const
    {
        return *this <= SIMDFloat(rhs);
    }

    SIMDFloat operator&(const SIMDFloat& rhs) const
    {
        return _mm256_and_ps(m_avx, rhs.m_avx);
    }

    SIMDFloat operator|(const SIMDFloat& rhs) const
    {
        return _mm256_or_ps(m_avx, rhs.m_avx);
    }

    SIMDFloat operator==(const SIMDFloat& rhs) const
    {
        return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_EQ_US);
    }

    SIMDFloat operator/(const SIMDFloat& rhs) const
    {
        return SIMDFloat(m_avx) * SIMDFloat(_mm256_rcp_ps(rhs.m_avx));
    }

    SIMDFloat operator/(const float rhs) const
    {
        return *this * (1.0f / rhs);
    }

    SIMDFloat floor()
    {
        return _mm256_floor_ps(m_avx);
    }


    SIMDFloat sqrt() const
    {
        return _mm256_sqrt_ps(m_avx);
    }

    SIMDFloat pow(const SIMDFloat& rhs) const
    {
        return _mm256_pow_ps(m_avx, rhs.m_avx);
    }


    union
    {
        __m256 m_avx;
        float v[8];
    };
};
#else

// SIMDFloat polyfill
class SIMDFloat
{
public:
    SIMDFloat()
    {
        for (int i = 0; i < 8; ++i) {
            v[i] = 0.0f;
        }
    }

    SIMDFloat(float val)
    {
        for (int i = 0; i < 8; ++i) {
            v[i] = val;
        }
    }

    SIMDFloat(const SIMDFloat& rhs)
    {
        for (int i = 0; i < 8; ++i) {
            v[i] = rhs.v[i];
        }
    }

    SIMDFloat(float a[8])
    {
        for (int i = 0; i < 8; ++i) {
            v[i] = a[i];
        }
    }

    SIMDFloat& operator=(const SIMDFloat& rhs)
    {
        for (int i = 0; i < 8; ++i) {
            v[i] = rhs.v[i];
        }
        return *this;
    }

    SIMDFloat operator*(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result.v[i] = v[i] * rhs.v[i];
        }
        return result;
    }

    SIMDFloat operator+(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result.v[i] = v[i] + rhs.v[i];
        }
        return result;
    }

    SIMDFloat operator-(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result.v[i] = v[i] - rhs.v[i];
        }
        return result;
    }

    SIMDFloat operator/(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result.v[i] = v[i] / rhs.v[i];
        }
        return result;
    }


    SIMDFloat operator>=(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = v[i] >= rhs.v[i] ? 1.0f : 0.0f;
        return result;
    }

    SIMDFloat operator>(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = v[i] > rhs.v[i] ? 1.0f : 0.0f;
        return result;
    }

    SIMDFloat operator<(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = v[i] < rhs.v[i] ? 1.0f : 0.0f;
        return result;
    }

    SIMDFloat operator<=(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = v[i] <= rhs.v[i] ? 1.0f : 0.0f;
        return result;
    }

    SIMDFloat operator==(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = v[i] == rhs.v[i] ? 1.0f : 0.0f;
        return result;
    }

    // Bitwise operators
    SIMDFloat operator&(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = static_cast<float>(static_cast<int>(v[i]) & static_cast<int>(rhs.v[i]));
        return result;
    }

    SIMDFloat operator|(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = static_cast<float>(static_cast<int>(v[i]) | static_cast<int>(rhs.v[i]));
        return result;
    }

    // Mathematical functions
    SIMDFloat floor() const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = std::floor(v[i]);
        return result;
    }

    SIMDFloat sqrt() const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = std::sqrt(v[i]);
        return result;
    }

    SIMDFloat pow(const SIMDFloat& rhs) const
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i)
            result.v[i] = std::pow(v[i], rhs.v[i]);
        return result;
    }

    bool GetBit(const size_t i) const
    {
        return v[i] == 1.0f; // Check the sign bit (most significant bit)
    }

    // Accessor
    float operator[](const size_t i) const
    {
        return v[i];
    }

    float& operator[](const size_t i)
    {
        return v[i];
    }

    // Other methods as needed

private:
    float v[8];
};

#endif

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

    SIMDVec2(Vec2 v) : x(v.x), y(v.y)
    {
    }


    SIMDVec2 operator+(const SIMDVec2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    SIMDVec2 operator-(const SIMDVec2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    SIMDVec2 operator*(const SIMDFloat& rhs) const
    {
        return { x * rhs, y * rhs };
    }

    SIMDVec2 operator/(const SIMDFloat& rhs) const
    {
        return { x / rhs, y / rhs };
    }

    SIMDFloat length() const
    {
        SIMDFloat squared = x * x + y * y;
        return squared.sqrt();
    }

    SIMDFloat Dot(const SIMDVec2& rhs)
    {
        return x * rhs.x + y * rhs.y;
    }

    SIMDVec2 Normalize()
    {
        return *this / length();
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

    SIMDVec3(Vec3 v) : x(v.x), y(v.y), z(v.z)
    {
    }

    SIMDVec3 operator+(const SIMDVec3& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    SIMDVec3 operator*(const SIMDVec3& rhs) const
    {
        return { x * rhs.x, y * rhs.y, z * rhs.z };
    }

    SIMDVec3 operator-(const SIMDVec3& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    SIMDVec3 operator*(const SIMDFloat& rhs) const
    {
        return { x * rhs, y * rhs, z * rhs };
    }

    SIMDVec3 operator/(const SIMDFloat& rhs) const
    {
        return { x / rhs, y / rhs, z / rhs };
    }

    SIMDFloat length() const
    {
        SIMDFloat squared = x * x + y * y + z * z;
        return squared.sqrt();
    }

    SIMDFloat Dot(const SIMDVec3& rhs)
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    SIMDVec3 Normalize()
    {
        return *this / length();
    }

    SIMDFloat x{}, y{}, z{};
};


namespace SIMD
{

    static SIMDFloat ONE = 1.0;
    static SIMDFloat ZERO = 0.0;

#ifdef AVX 
    inline bool All(const SIMDFloat& rhs)
    {
        return _mm256_movemask_ps(rhs.m_avx) == 255;
    };

    inline bool Any(const SIMDFloat& rhs)
    {
        return _mm256_movemask_ps(rhs.m_avx) != 0x0;
    };

    // if mask bit == 0 than A bit will be chosen
    // if mask bit == 1 than B bit will be chosen
    inline SIMDFloat Select(const SIMDFloat& mask, SIMDFloat& a, SIMDFloat& b)
    {
        return _mm256_blendv_ps(a.m_avx, b.m_avx, mask.m_avx);
    }

    // get max between a and b
    inline SIMDFloat Max(SIMDFloat a, SIMDFloat b)
    {
        return Select(a > b, b, a);
    }

    inline SIMDFloat Min(SIMDFloat a, SIMDFloat b)
    {
        return Select(a > b, a, b);
    }
#else
    inline bool All(const SIMDFloat& rhs)
    {
        for (int i = 0; i < 8; ++i) {
            if (rhs[i] == 0.0f) {
                return false;
            }
        }
        return true;
    }

    inline bool Any(const SIMDFloat& rhs)
    {
        for (int i = 0; i < 8; ++i) {
            if (rhs[i] != 0.0f) {
                return true;
            }
        }
        return false;
    }

    inline SIMDFloat Select(const SIMDFloat& mask, const SIMDFloat& a, const SIMDFloat& b)
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result[i] = (mask[i] != 0.0f) ? b[i] : a[i];
        }
        return result;
    }

    inline SIMDFloat Max(const SIMDFloat& a, const SIMDFloat& b)
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result[i] = (a[i] > b[i]) ? a[i] : b[i];
        }
        return result;
    }

    inline SIMDFloat Min(const SIMDFloat& a, const SIMDFloat& b)
    {
        SIMDFloat result;
        for (int i = 0; i < 8; ++i) {
            result[i] = (a[i] < b[i]) ? a[i] : b[i];
        }
        return result;
    }

#endif
    // clamp c between a and b
    // require: a < b
    inline SIMDFloat Clamp(SIMDFloat a, SIMDFloat b, SIMDFloat c)
    {
        return Min(b, Max(a, c));
    }
};
