#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"

/*
* /brief multithreaded renderer
*/
class RendererM : public System {
public:
    RendererM() {}

    void Render();

    void VertexShaderStage();

    void ClippingStage();

    void TiledRasterizationStage();
    
    void RasterizationStage();

    void FragmentShaderStage();

    void UpdateFrameBuffer();

};