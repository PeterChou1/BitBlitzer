#include "stdafx.h"
#include "SIMDPixel.h"


static float offsetX[8] = { 0.0f, 1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 2.0f, 3.0f };
static float offsetY[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

SIMDFloat SIMDPixel::PixelOffsetX = SIMDFloat(offsetX);
SIMDFloat SIMDPixel::PixelOffsetY = SIMDFloat(offsetY);