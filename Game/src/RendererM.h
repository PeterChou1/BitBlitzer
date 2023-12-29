#pragma once
#include <vector>
#include "Mesh.h"
#include "Mat4.h"
#include "System.h"
#include "SimpleTexture.h"
#include "DepthBuffer.h"
#include "GraphicsBuffer.h"
#include "Camera.h"
#include "ColorBuffer.h"

/*
* /brief multithreaded renderer
*/
class RendererM : public System {
public:
    RendererM(GraphicsBuffer& g, Camera& cam, ColorBuffer& color);

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
    Camera& m_cam;
    ColorBuffer& m_color;
    int m_coreCount;
    int m_coreInterval;
};