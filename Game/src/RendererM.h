#pragma once
#include <vector>
#include "Mesh.h"
#include "System.h"
#include "DepthBuffer.h"
#include "GraphicsBuffer.h"
#include "Camera.h"
#include "ColorBuffer.h"
#include "SimpleTexture.h"

/*
* /brief multithreaded renderer
*/
class RendererM : public System
{
public:
    RendererM(GraphicsBuffer& g, Camera& cam, ColorBuffer& color, DepthBufferSIMD& depth, TextureList& textureList);

    void Render();

    void VertexShaderStage();

    void ClippingStage();

    void TiledRasterizationStage();

    void RasterizationStage();

    void FragmentShaderStage();

    void UpdateFrameBuffer();

    void ClearBuffer();

private:
    std::vector<std::uint32_t> m_coreIds;
    std::vector<Vertex>& m_vertexBuffer;
    std::vector<std::uint32_t>& m_indexBuffer;
    std::vector<Vertex>& m_projectedVertexBuffer;
    std::vector<std::vector<Triangle>>& m_projectedClip;
    std::vector<Tile>& m_tiles;
    TextureList& m_textureList;
    Camera& m_cam;
    ColorBuffer& m_color;
    DepthBufferSIMD& m_depth;
    unsigned int m_coreCount;
    int m_coreInterval;
};
