#include "stdafx.h"
#include "SIMDPixel.h"


float PixelOffsetX[8] = { 0.0f, 1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 2.0f, 3.0f };
float PixelOffsetY[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
SIMDFloat SIMDPixel::PixelOffsetX = SIMDFloat(PixelOffsetX);
SIMDFloat SIMDPixel::PixelOffsetY = SIMDFloat(PixelOffsetY);