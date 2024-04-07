//---------------------------------------------------------------------------------
// RenderDirect.h
//---------------------------------------------------------------------------------
// Demonstrator on how we could speed up our Rendering see FragmentShader.cpp for more
// information
// NOTE: This code is not actually used in code but rather serve as demonstration on how to
//       bypass the bottleneck provided by the native API

#pragma once

namespace DirectRender
{

    void SetupGL();

    void RenderTexture(unsigned char* imageData);

}
