//---------------------------------------------------------------------------------
// FragmentShader.h
//---------------------------------------------------------------------------------
//
// Multi-threaded Shader that shades the screen on cores
// while shading 8 pixels at a time using AVX2 instructions
//
#pragma once
#include "Camera.h"
#include "ClippedTriangleBuffer.h"
#include "ColorBuffer.h"
#include "CubeMap.h"
#include "Lighting.h"
#include "PixelBuffer.h"

class FragmentShader
{
public:

    FragmentShader();

    void Shade();

private:
    std::shared_ptr<ClippedTriangleBuffer> m_ClippedTriangle;
    std::shared_ptr<PixelBuffer> m_PixelBuffer;
    std::shared_ptr<ColorBuffer> m_ColorBuffer;
    std::shared_ptr<CubeMap> m_CubeMap;
    std::shared_ptr<Camera> m_Cam;
    std::shared_ptr<Lighting> m_Light;

};
