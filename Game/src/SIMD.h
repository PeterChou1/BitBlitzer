#pragma once
#include <immintrin.h>
#include <iostream>

/* 
* /brief Float based on avx2
*/
 class SIMDFloat 
 {
 public:
     SIMDFloat() : m_avx(_mm256_setzero_ps()) {}
 
     SIMDFloat(__m256 avx) : m_avx(avx) {}
 
     SIMDFloat(const float& a) : m_avx(_mm256_broadcast_ss(&a)) {}
 
     SIMDFloat(float* a) : m_avx(_mm256_loadu_ps(a)) {}
 
     inline SIMDFloat operator*(const SIMDFloat& rhs) { return SIMDFloat(_mm256_mul_ps(m_avx, rhs.m_avx)); };
 
     inline SIMDFloat operator+(const SIMDFloat& rhs) { return SIMDFloat(_mm256_add_ps(m_avx, rhs.m_avx)); };
 
     inline SIMDFloat operator-(const SIMDFloat& rhs) { return SIMDFloat(_mm256_sub_ps(m_avx, rhs.m_avx)); };
 
     inline float& operator [](const size_t i) { return v[i]; }
     inline bool GetBit(const size_t i) const { return (_mm256_movemask_ps(m_avx) >> i) & 1; }
 
     inline SIMDFloat operator >=(const SIMDFloat& rhs) const { return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLT_US); }
     inline SIMDFloat operator >=(const float& rhs) const { return *this >= SIMDFloat(rhs); }
 
     inline SIMDFloat operator >(const SIMDFloat& rhs) const { return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_NLE_US); }
     inline SIMDFloat operator >(const float& rhs) const { return *this > SIMDFloat(rhs); }
 
     inline SIMDFloat operator <(const SIMDFloat& rhs) const { return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LT_OS); }
     inline SIMDFloat operator <(const float& rhs) const { return *this < SIMDFloat(rhs); }
 
     inline SIMDFloat operator <=(const SIMDFloat& rhs) const { return _mm256_cmp_ps(m_avx, rhs.m_avx, _CMP_LE_OS); }
     inline SIMDFloat operator <=(const float& rhs) const { return *this <= SIMDFloat(rhs); }
 
     inline SIMDFloat operator &(const SIMDFloat& rhs) const { return _mm256_and_ps(m_avx, rhs.m_avx); }
     inline SIMDFloat operator |(const SIMDFloat& rhs) const { return _mm256_or_ps(m_avx, rhs.m_avx); }
 
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

     SIMDVec2(float x, float y) : x(SIMDFloat(x)), y(SIMDFloat(y)) {}

     SIMDVec2(SIMDFloat x, SIMDFloat y) : x(x), y(y) {}
 
     SIMDFloat x{}, y{};

     inline SIMDVec2 operator+(const SIMDVec2& rhs) { return SIMDVec2(x + rhs.x, y + rhs.y); }

 };


 /*
 * /brief sets of 8 pixels (4 x 2) 
 */
 struct SIMDPixel
 {
     const static int pixelWidth = 4;
     const static int pixelHeight = 2;

     SIMDVec2 position{};
     SIMDFloat mask{};

     SIMDPixel(int pixelX, int pixelY)
     {
         // offset for pixel
         SIMDFloat x = SIMDFloat({ 0.0f, 1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 2.0f, 3.0f });
         SIMDFloat y = SIMDFloat({ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f });
         position = SIMDVec2(static_cast<float>(pixelX), static_cast<float>(pixelY)) + SIMDVec2(x, y);
     };
 };


 
 namespace SIMD
 {
 
     inline bool All(const SIMDFloat& rhs) { return _mm256_movemask_ps(rhs.m_avx) == 0xf; };
 
     inline bool Any(const SIMDFloat& rhs) { return _mm256_movemask_ps(rhs.m_avx) != 0x0; };
 
 };
