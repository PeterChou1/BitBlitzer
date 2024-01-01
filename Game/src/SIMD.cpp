#include "stdafx.h"
#include "SIMD.h"

SIMDFloat SIMDPixel::PixelOffsetX = SIMDFloat({ 0.0f, 1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 2.0f, 3.0f });

SIMDFloat SIMDPixel::PixelOffsetY = SIMDFloat({ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f });
